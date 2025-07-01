// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/BotPerceptionComponent.h"
#include "Characters/AICharacter.h"
#include "Controllers/BotController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionTypes.h"
#include "Components/BotTeamComponent.h"
#include "Components/BotHealthComponent.h"
#include "Components/BotWeaponComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "LogBotArena.h"
#include "Utils/BotArenaUtils.h"
#include "DrawDebugHelpers.h"

UBotPerceptionComponent::UBotPerceptionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Default values
    SelectTargetInterval = 5.0f;
    SelectTargetRotationSpeed = 1.0f;
    TimeSinceTargetSelection = 0.0f;
}

void UBotPerceptionComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize perception when the component begins play
    InitializePerception();
}

void UBotPerceptionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (!IsValid(GetOwner()))
    {
        UE_LOG(LogBotArena, Warning, TEXT("TickComponent: Invalid owner"));
        return;
    }
    
    // Update time since last target selection
    TimeSinceTargetSelection += DeltaTime;
    
    // Process perception updates in the game thread
    if (bHasPendingPerceptionUpdate)
    {
        TArray<AActor*> LocalSensedActors;
        {
            // Use a critical section to safely copy the data
            FScopeLock Lock(&PerceptionLock);
            LocalSensedActors = PendingSensedActors;
            bHasPendingPerceptionUpdate = false;
        }
        
        // Process the perception update in the game thread
        SelectTarget(LocalSensedActors);
    }
    
    // Handle smooth rotation towards target
    ABotController* BotController = GetBotController();
    if (!BotController)
    {
        return;
    }
    
    AActor* SelectedTarget = GetSelectedTarget();
    if (!SelectedTarget)
    {
        return;
    }
    
    AActor* PossessedActor = BotController->GetPawn();
    if (!PossessedActor)
    {
        return;
    }
    
    FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(
        PossessedActor->GetActorLocation(), 
        SelectedTarget->GetActorLocation()
    );
    
    PossessedActor->SetActorRotation(FMath::RInterpTo(
        PossessedActor->GetActorRotation(), 
        TargetRotation, 
        DeltaTime, 
        SelectTargetRotationSpeed
    ));
    
    // Draw debug visualization
    DebugDrawPerception(DeltaTime);
}

void UBotPerceptionComponent::InitializePerception()
{
    ABotController* BotController = GetBotController();
    if (!BotController)
    {
        return;
    }
    
    // Get the perception component from the controller
    PerceptionComp = BotController->GetPerceptionComponent();
    if (PerceptionComp)
    {
        // Register for perception updates
        PerceptionComp->OnPerceptionUpdated.AddDynamic(this, &UBotPerceptionComponent::OnPerceptionUpdated);
        
        // Configure sight sense
        UAISenseConfig_Sight* SightConfig = Cast<UAISenseConfig_Sight>(PerceptionComp->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>()));
        if (SightConfig)
        {
            SightConfig->DetectionByAffiliation.bDetectEnemies = true;
            SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
            SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
        }
    }
}

void UBotPerceptionComponent::SelectTarget(const TArray<AActor*>& TargetList)
{
    if (!IsValid(GetOwner()))
    {
        UE_LOG(LogBotArena, Warning, TEXT("SelectTarget: Invalid owner"));
        return;
    }
    
    ABotController* BotController = GetBotController();
    if (!BotController)
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: No BotController"), *GetNameSafe(GetOwner()));
        return;
    }
    
    if (TargetList.Num() <= 0)
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: Empty target list"), *GetNameSafe(GetOwner()));
        return;
    }
    
    AAICharacter* ControlledCharacter = Cast<AAICharacter>(BotController->GetPawn());
    if (!ControlledCharacter)
    {
        UE_LOG(LogBotArena, Warning, TEXT("%s: No controlled character"), *GetNameSafe(GetOwner()));
        return;
    }
    
    // Only select a new target if we don't have one or enough time has passed
    AActor* CurrentTarget = GetSelectedTarget();
    bool bNeedsNewTarget = !CurrentTarget || TimeSinceTargetSelection >= SelectTargetInterval;
    
    if (!bNeedsNewTarget)
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: Skipping target selection, current target: %s, time since selection: %.2f"), 
               *GetNameSafe(GetOwner()), *GetNameSafe(CurrentTarget), TimeSinceTargetSelection);
        return;
    }
    
    UE_LOG(LogBotArena, Verbose, TEXT("%s: Selecting new target from %d sensed actors"), 
           *GetNameSafe(GetOwner()), TargetList.Num());
    
    // Search for the closest target
    float ClosestDistance = 99999.0f;
    AAICharacter* SelectedTarget = nullptr;
    FVector CharacterLocation = ControlledCharacter->GetActorLocation();
    
    // Get team component once for efficiency
    UBotTeamComponent* TeamComp = FBotArenaUtils::GetComponentSafe<UBotTeamComponent>(ControlledCharacter, TEXT("TeamComponent"));
    if (!TeamComp)
    {
        UE_LOG(LogBotArena, Warning, TEXT("%s: Missing TeamComponent"), *GetNameSafe(ControlledCharacter));
        return;
    }
    
    int32 PotentialTargetsCount = 0;
    
    // Choose a target
    for (AActor* Target : TargetList)
    {
        if (!IsValid(Target))
        {
            continue;
        }
        
        AAICharacter* Bot = Cast<AAICharacter>(Target);
        if (!Bot)
        {
            continue;
        }
        
        if (!Bot->IsAlive())
        {
            continue;
        }
        
        if (!TeamComp->IsHostile(Bot))
        {
            continue;
        }
        
        PotentialTargetsCount++;
        
        // Calculate distance
        float Distance = FVector::Dist(Bot->GetActorLocation(), CharacterLocation);
        
        // We have a new target if it's closer
        if (Distance < ClosestDistance)
        {
            ClosestDistance = Distance;
            SelectedTarget = Bot;
        }
    }
    
    UE_LOG(LogBotArena, Verbose, TEXT("%s: Found %d potential targets"), 
           *GetNameSafe(GetOwner()), PotentialTargetsCount);
    
    // Update blackboard with the selected target
    if (SelectedTarget)
    {
        UE_LOG(LogBotArena, Log, TEXT("%s: Selected target %s at distance %.2f"), 
               *GetNameSafe(GetOwner()), *GetNameSafe(SelectedTarget), ClosestDistance);
        
        UBlackboardComponent* BlackboardComp = BotController->GetBlackboardComponent();
        if (BlackboardComp)
        {
            BlackboardComp->SetValueAsObject(FName("SelectedTarget"), SelectedTarget);
            
            // Reset timer and broadcast event
            TimeSinceTargetSelection = 0.0f;
            OnTargetSelected.Broadcast(SelectedTarget);
        }
        else
        {
            UE_LOG(LogBotArena, Warning, TEXT("%s: Missing BlackboardComponent"), *GetNameSafe(BotController));
        }
    }
    else
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: No suitable target found"), *GetNameSafe(GetOwner()));
    }
}

AActor* UBotPerceptionComponent::GetSelectedTarget() const
{
    ABotController* BotController = GetBotController();
    if (BotController)
    {
        UBlackboardComponent* BlackboardComp = BotController->GetBlackboardComponent();
        if (BlackboardComp)
        {
            UObject* Target = BlackboardComp->GetValueAsObject(FName("SelectedTarget"));
            return Cast<AActor>(Target);
        }
    }
    
    return nullptr;
}

FVector UBotPerceptionComponent::GetSelectedTargetLocation() const
{
    AActor* SelectedTarget = GetSelectedTarget();
    if (SelectedTarget)
    {
        return SelectedTarget->GetActorLocation();
    }
    
    return FVector::ForwardVector;
}

void UBotPerceptionComponent::DebugDrawPerception(float DeltaTime)
{
#if ENABLE_DRAW_DEBUG
    // Only draw debug info in development builds
    static const auto CVarDebugPerception = IConsoleManager::Get().FindConsoleVariable(TEXT("BotArena.DebugPerception"));
    
    // If the console variable doesn't exist yet, create it
    static bool bInitialized = false;
    if (!bInitialized)
    {
        if (!CVarDebugPerception)
        {
            IConsoleManager::Get().RegisterConsoleVariable(
                TEXT("BotArena.DebugPerception"),
                0,
                TEXT("Enable debug visualization for bot perception: 0=off, 1=on"),
                ECVF_Cheat);
        }
        bInitialized = true;
    }
    
    // Check if debug visualization is enabled
    if (CVarDebugPerception && CVarDebugPerception->GetInt() > 0)
    {
        AAICharacter* Character = GetAICharacterOwner();
        if (!Character)
        {
            return;
        }
        
        UWorld* World = GetWorld();
        if (!World)
        {
            return;
        }
        
        // Draw perception radius
        const float PerceptionRadius = 500.0f; // This should ideally match the actual perception radius
        DrawDebugSphere(World, Character->GetActorLocation(), PerceptionRadius, 16, FColor::Yellow, false, DeltaTime * 1.1f);
        
        // Draw line to target if any
        AActor* Target = GetSelectedTarget();
        if (Target)
        {
            DrawDebugLine(World, Character->GetActorLocation(), Target->GetActorLocation(), 
                          FColor::Red, false, DeltaTime * 1.1f, 0, 2.0f);
            
            // Draw box around target
            FVector TargetLocation = Target->GetActorLocation();
            DrawDebugBox(World, TargetLocation, FVector(50.0f), FColor::Red, false, DeltaTime * 1.1f);
            
            // Draw target info
            FString TargetInfo = FString::Printf(TEXT("%s\nDist: %.1f"), 
                                               *Target->GetName(), 
                                               FVector::Dist(Character->GetActorLocation(), TargetLocation));
            DrawDebugString(World, TargetLocation + FVector(0, 0, 100), TargetInfo, nullptr, FColor::White, DeltaTime * 1.1f);
        }
        
        // Draw bot info
        FString BotInfo = FString::Printf(TEXT("%s\nHealth: %.1f\nAmmo: %d"), 
                                        *Character->GetName(), 
                                        Character->GetHealthComponent() ? Character->GetHealthComponent()->GetHealth() : 0.0f,
                                        Character->GetWeaponComponent() ? Character->GetWeaponComponent()->GetCurrentAmmo() : 0);
        DrawDebugString(World, Character->GetActorLocation() + FVector(0, 0, 100), BotInfo, nullptr, FColor::Green, DeltaTime * 1.1f);
    }
#endif
}

void UBotPerceptionComponent::OnPerceptionUpdated(const TArray<AActor*>& SensedActors)
{
    // Use a critical section to prevent race conditions
    FScopeLock Lock(&PerceptionLock);
    
    // Store a copy of the sensed actors
    PendingSensedActors = SensedActors;
    bHasPendingPerceptionUpdate = true;
    
    UE_LOG(LogBotArena, Verbose, TEXT("%s: Perception updated with %d actors"), 
           *GetNameSafe(GetOwner()), SensedActors.Num());
}