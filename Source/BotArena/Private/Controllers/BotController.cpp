// Fill out your copyright notice in the Description page of Project Settings.

#include "Controllers/BotController.h"
#include "Components/BotPerceptionComponent.h"
#include "Components/BotBehaviorComponent.h"
#include "Components/BotPathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "MiscClasses/AmmoBox.h"

ABotController::ABotController()
{
    // Create components
    BotPerceptionComponent = CreateDefaultSubobject<UBotPerceptionComponent>(TEXT("BotPerceptionComponent"));
    BotBehaviorComponent = CreateDefaultSubobject<UBotBehaviorComponent>(TEXT("BotBehaviorComponent"));
    
    // Create the AI perception component (kept for compatibility)
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
    if (PerceptionComp)
    {
        SetPerceptionComponent(*PerceptionComp);
    }
    
    // Create stimuli source component (kept for compatibility)
    StimuliSourceComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSourceComp"));
    
    // Create path following component
    BotPathFollowingComp = CreateDefaultSubobject<UBotPathFollowingComponent>(TEXT("BotPathFollowingComp"));
}

void ABotController::BeginPlay()
{
    Super::BeginPlay();
    
    // Set the path following component
    if (BotPathFollowingComp)
    {
        SetPathFollowingComponent(BotPathFollowingComp);
    }
}

void ABotController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABotController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    // Initialize behavior
    if (BotBehaviorComponent)
    {
        BotBehaviorComponent->InitializeBehavior();
    }
    
    // Initialize perception
    if (BotPerceptionComponent)
    {
        BotPerceptionComponent->InitializePerception();
    }
}

void ABotController::OnUnPossess()
{
    Super::OnUnPossess();
    
    // By default the controller will stay in the level so manually destroy this actor
    Destroy();
}

FVector ABotController::GetSelectedTargetLocation() const
{
    if (BotPerceptionComponent)
    {
        return BotPerceptionComponent->GetSelectedTargetLocation();
    }
    
    return FVector::ForwardVector;
}

void ABotController::SetMoveToLocation(const FVector& Location)
{
    if (BotBehaviorComponent)
    {
        BotBehaviorComponent->SetMoveToLocation(Location);
    }
}

void ABotController::SetAmmoBox(AAmmoBox* AmmoBox)
{
    if (BotBehaviorComponent)
    {
        BotBehaviorComponent->SetAmmoBox(AmmoBox);
    }
}

UBehaviorTree* ABotController::GetCurrentTree()
{
    if (BotBehaviorComponent)
    {
        return BotBehaviorComponent->GetBehaviorTree();
    }
    
    return nullptr;
}

AActor* ABotController::GetSelectedTarget() const
{
    if (BotPerceptionComponent)
    {
        return BotPerceptionComponent->GetSelectedTarget();
    }
    
    return nullptr;
}

void ABotController::InitiateRetreat()
{
    if (BotBehaviorComponent)
    {
        BotBehaviorComponent->InitiateRetreat();
    }
}

void ABotController::SetCollectAmmoStatus(bool NewStatus)
{
    if (BotBehaviorComponent)
    {
        BotBehaviorComponent->SetCollectAmmoStatus(NewStatus);
    }
}