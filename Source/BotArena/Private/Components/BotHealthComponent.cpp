// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/BotHealthComponent.h"
#include "Characters/AICharacter.h"
#include "Controllers/BotController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "MiscClasses/BotCounter.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "LogBotArena.h"
#include "Utils/BotArenaUtils.h"

UBotHealthComponent::UBotHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    // Default values
    MaxHealth = 100.0f;
    Health = MaxHealth;
    RetreatHealthPercentage = 0.2f;
    DestroyActorDelay = 5.0f;
}

void UBotHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize health to max health
    Health = MaxHealth;
}

float UBotHealthComponent::HandleDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (!IsValid(GetOwner()))
    {
        UE_LOG(LogBotArena, Warning, TEXT("HandleDamage: Invalid owner"));
        return 0.0f;
    }
    
    // Validate damage value
    if (Damage <= 0)
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: Received invalid damage value: %f"), 
               *GetNameSafe(GetOwner()), Damage);
        return 0.0f;
    }
    
    // Check if already dead
    if (!IsAlive())
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: Received damage while already dead"), 
               *GetNameSafe(GetOwner()));
        return 0.0f;
    }
    
    // Log damage source if available
    if (DamageCauser)
    {
        UE_LOG(LogBotArena, Log, TEXT("%s: Taking %.2f damage from %s"), 
               *GetNameSafe(GetOwner()), Damage, *GetNameSafe(DamageCauser));
    }
    else
    {
        UE_LOG(LogBotArena, Log, TEXT("%s: Taking %.2f damage from unknown source"), 
               *GetNameSafe(GetOwner()), Damage);
    }
    
    // Calculate the actual damage (could be modified by armor, resistances, etc.)
    const float ActualDamage = Damage;
    
    // Apply damage
    const float OldHealth = Health;
    Health = FMath::Max(Health - ActualDamage, 0.0f);
    
    // Broadcast health changed event
    OnHealthChanged.Broadcast(Health, OldHealth - Health);
    
    UE_LOG(LogBotArena, Log, TEXT("%s: Health changed from %.2f to %.2f"), 
           *GetNameSafe(GetOwner()), OldHealth, Health);
    
    // Check if we should retreat
    if (ShouldRetreat())
    {
        UE_LOG(LogBotArena, Log, TEXT("%s: Health low, initiating retreat"), 
               *GetNameSafe(GetOwner()));
               
        ABotController* BotController = GetBotController();
        if (BotController)
        {
            BotController->InitiateRetreat();
        }
        else
        {
            UE_LOG(LogBotArena, Warning, TEXT("%s: No BotController found for retreat"), 
                   *GetNameSafe(GetOwner()));
        }
    }
    
    // Check if we died
    if (!IsAlive())
    {
        UE_LOG(LogBotArena, Log, TEXT("%s: Health depleted, handling death"), 
               *GetNameSafe(GetOwner()));
        HandleDeath();
    }
    
    return ActualDamage;
}

bool UBotHealthComponent::IsAlive() const
{
    return Health > 0.0f;
}

bool UBotHealthComponent::ShouldRetreat() const
{
    return IsAlive() && Health <= MaxHealth * RetreatHealthPercentage;
}

void UBotHealthComponent::HandleDeath()
{
    UE_LOG(LogBotArena, Log, TEXT("%s: Handling death"), *GetNameSafe(GetOwner()));
    
    // Broadcast death event
    OnDeath.Broadcast();
    
    AAICharacter* Character = GetAICharacterOwner();
    if (!Character)
    {
        UE_LOG(LogBotArena, Warning, TEXT("HandleDeath: No AICharacter owner"));
        return;
    }
    
    // If the bot was crouching while it died, uncrouch first to avoid "funny" ragdoll effects
    UCharacterMovementComponent* MovementComp = FBotArenaUtils::GetComponentSafe<UCharacterMovementComponent>(Character, TEXT("CharacterMovementComponent"));
    if (MovementComp)
    {
        if (MovementComp->IsCrouching())
        {
            UE_LOG(LogBotArena, Verbose, TEXT("%s: Uncrouching before death"), *GetNameSafe(Character));
            MovementComp->UnCrouch();
        }
    }
    else
    {
        UE_LOG(LogBotArena, Warning, TEXT("%s: Missing CharacterMovementComponent"), *GetNameSafe(Character));
    }
    
    // Enable ragdoll physics
    USkeletalMeshComponent* Mesh = Character->GetMesh();
    if (Mesh)
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: Enabling ragdoll physics"), *GetNameSafe(Character));
        Mesh->SetSimulatePhysics(true);
        Mesh->SetCollisionProfileName(FName("Ragdoll"));
        Mesh->SetCollisionResponseToAllChannels(ECR_Block);
    }
    else
    {
        UE_LOG(LogBotArena, Warning, TEXT("%s: Missing Mesh component"), *GetNameSafe(Character));
    }
    
    // Allow other pawns to walk over the dead character
    UCapsuleComponent* CapsuleComp = Character->GetCapsuleComponent();
    if (CapsuleComp)
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: Adjusting capsule collision"), *GetNameSafe(Character));
        CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    }
    else
    {
        UE_LOG(LogBotArena, Warning, TEXT("%s: Missing CapsuleComponent"), *GetNameSafe(Character));
    }
    
    // Unpossess the character
    ABotController* BotController = GetBotController();
    if (BotController)
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: Updating bot counter and unpossessing"), *GetNameSafe(Character));
        
        // Find the BotCounter to update it
        ABotCounter* BotCounter = nullptr;
        TArray<AActor*> BotCounterArray;
        UWorld* World = GetWorld();
        
        if (!World)
        {
            UE_LOG(LogBotArena, Warning, TEXT("%s: Invalid World"), *GetNameSafe(Character));
        }
        else
        {
            UGameplayStatics::GetAllActorsOfClass(World, ABotCounter::StaticClass(), BotCounterArray);
            if (BotCounterArray.IsValidIndex(0))
            {
                BotCounter = Cast<ABotCounter>(BotCounterArray[0]);
            }
            
            // Update the bot counter if found
            if (BotCounter)
            {
                // Get the team from the character
                ETeam Team = Character->GetTeam();
                UE_LOG(LogBotArena, Verbose, TEXT("%s: Updating bot counter for team %d"), 
                       *GetNameSafe(Character), (int32)Team);
                BotCounter->OnBotDeath(Team);
            }
            else
            {
                UE_LOG(LogBotArena, Warning, TEXT("%s: No BotCounter found in level"), *GetNameSafe(Character));
            }
            
            // Destroy the actor after a delay using a weak pointer for safety
            FTimerHandle DestroyTimer;
            FTimerDelegate DestroyDelegate;
            
            // Use a weak pointer to avoid dangling references if the character is destroyed early
            TWeakObjectPtr<AAICharacter> WeakCharacter(Character);
            
            DestroyDelegate.BindLambda([WeakCharacter, this]() {
                if (WeakCharacter.IsValid())
                {
                    UE_LOG(LogBotArena, Log, TEXT("%s: Destroying character after death delay"), 
                           *GetNameSafe(WeakCharacter.Get()));
                    WeakCharacter->Destroy();
                }
                else
                {
                    UE_LOG(LogBotArena, Warning, TEXT("HandleDeath: Character already destroyed before timer expired"));
                }
            });
            
            World->GetTimerManager().SetTimer(DestroyTimer, DestroyDelegate, DestroyActorDelay, false);
        }
        
        BotController->UnPossess();
    }
    else
    {
        UE_LOG(LogBotArena, Warning, TEXT("%s: No BotController found"), *GetNameSafe(Character));
    }
}