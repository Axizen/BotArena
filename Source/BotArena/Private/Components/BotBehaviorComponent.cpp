// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/BotBehaviorComponent.h"
#include "Controllers/BotController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "MiscClasses/AmmoBox.h"

UBotBehaviorComponent::UBotBehaviorComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    // Initialize blackboard key names
    BlackboardKey_MoveLocation = FName("MoveLocation");
    BlackboardKey_SelectedTarget = FName("SelectedTarget");
    BlackboardKey_ShouldRetreat = FName("ShouldRetreat");
    BlackboardKey_CollectAmmo = FName("CollectAmmo");
    BlackboardKey_AmmoBox = FName("AmmoBox");
}

void UBotBehaviorComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize behavior when the component begins play
    InitializeBehavior();
}

void UBotBehaviorComponent::InitializeBehavior()
{
    ABotController* BotController = GetBotController();
    if (!BotController || !BTAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid BotController or BTAsset in BotBehaviorComponent"));
        return;
    }
    
    // Run the behavior tree
    BotController->RunBehaviorTree(BTAsset);
}

void UBotBehaviorComponent::SetMoveToLocation(const FVector& Location)
{
    ABotController* BotController = GetBotController();
    if (!BotController)
    {
        return;
    }
    
    UBlackboardComponent* BlackboardComp = BotController->GetBlackboardComponent();
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsVector(BlackboardKey_MoveLocation, Location);
    }
}

void UBotBehaviorComponent::SetAmmoBox(AAmmoBox* AmmoBox)
{
    ABotController* BotController = GetBotController();
    if (!BotController)
    {
        return;
    }
    
    UBlackboardComponent* BlackboardComp = BotController->GetBlackboardComponent();
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsObject(BlackboardKey_AmmoBox, AmmoBox);
    }
}

void UBotBehaviorComponent::InitiateRetreat()
{
    ABotController* BotController = GetBotController();
    if (!BotController)
    {
        return;
    }
    
    UBlackboardComponent* BlackboardComp = BotController->GetBlackboardComponent();
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsBool(BlackboardKey_ShouldRetreat, true);
    }
}

void UBotBehaviorComponent::SetCollectAmmoStatus(bool NewStatus)
{
    ABotController* BotController = GetBotController();
    if (!BotController)
    {
        return;
    }
    
    UBlackboardComponent* BlackboardComp = BotController->GetBlackboardComponent();
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsBool(BlackboardKey_CollectAmmo, NewStatus);
    }
}