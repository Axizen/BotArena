// Fill out your copyright notice in the Description page of Project Settings.

#include "AIServices/BTService_CheckForAmmo.h"
#include "Characters/AICharacter.h"
#include "Controllers/BotController.h"
#include "Components/BotWeaponComponent.h"
#include "Components/BotBehaviorComponent.h"

void UBTService_CheckForAmmo::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    ABotController* BotController = Cast<ABotController>(OwnerComp.GetAIOwner());
    if (!BotController)
    {
        return;
    }

    AAICharacter* Bot = Cast<AAICharacter>(BotController->GetPawn());
    if (!Bot)
    {
        return;
    }

    // Check if the bot is low on ammo
    bool IsLowOnAmmo = false;
    
    // Try to use the weapon component first
    UBotWeaponComponent* WeaponComponent = Bot->GetWeaponComponent();
    if (WeaponComponent)
    {
        IsLowOnAmmo = WeaponComponent->LowOnAmmo();
    }
    else
    {
        // Fall back to the AICharacter method for backward compatibility
        IsLowOnAmmo = Bot->LowOnAmmo();
    }

    // Update the blackboard value
    UBotBehaviorComponent* BehaviorComponent = BotController->GetBotBehaviorComponent();
    if (BehaviorComponent)
    {
        BehaviorComponent->SetCollectAmmoStatus(IsLowOnAmmo);
    }
    else
    {
        // Fall back to the controller method for backward compatibility
        BotController->SetCollectAmmoStatus(IsLowOnAmmo);
    }
}