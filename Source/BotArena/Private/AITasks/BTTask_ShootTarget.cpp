// Fill out your copyright notice in the Description page of Project Settings.

#include "AITasks/BTTask_ShootTarget.h"
#include "AIController.h"
#include "Characters/AICharacter.h"
#include "Components/BotWeaponComponent.h"
#include "BehaviorTree/Tasks/BTTask_RunEQSQuery.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTTask_ShootTarget::UBTTask_ShootTarget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_ShootTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }
    
    AAICharacter* Bot = Cast<AAICharacter>(AIController->GetPawn());
    if (!Bot)
    {
        return EBTNodeResult::Failed;
    }
    
    // Try to get the weapon component first
    UBotWeaponComponent* WeaponComponent = Bot->GetWeaponComponent();
    if (WeaponComponent)
    {
        // Use the component directly
        WeaponComponent->FireWeapon();
        return EBTNodeResult::Succeeded;
    }
    else
    {
        // Fall back to the AICharacter method for backward compatibility
        Bot->FireWeapon();
        return EBTNodeResult::Succeeded;
    }
    
    return EBTNodeResult::Failed;
}