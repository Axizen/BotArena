// Fill out your copyright notice in the Description page of Project Settings.

#include "AITasks/BTTask_CollectAmmo.h"
#include "Characters/AICharacter.h"
#include "Controllers/BotController.h"
#include "Components/BotBehaviorComponent.h"
#include "CollisionQueryParams.h"
#include "WorldCollision.h"
#include "Engine/World.h"
#include "MiscClasses/AmmoBox.h"

EBTNodeResult::Type UBTTask_CollectAmmo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    AActor* Bot = OwnerComp.GetOwner();
    if (!Bot)
    {
        return EBTNodeResult::Failed;
    }

    // Set up collision parameters
    FCollisionObjectQueryParams CollisionObjectParams = FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldDynamic);

    FCollisionShape CollisionShape = FCollisionShape();
    CollisionShape.ShapeType = ECollisionShape::Sphere;
    CollisionShape.SetSphere(SearchRadius);

    TArray<FHitResult> OutHits;

    ABotController* BotController = Cast<ABotController>(OwnerComp.GetAIOwner());
    if (!BotController)
    {
        return EBTNodeResult::Failed;
    }

    // Perform a Sphere Sweep with the according radius to find a nearby Ammo box
    if (GetWorld()->SweepMultiByObjectType(OutHits, Bot->GetActorLocation(), Bot->GetActorLocation() + FVector(150.f), FQuat(), CollisionObjectParams, CollisionShape))
    {
        for (int32 Hit = 0; Hit < OutHits.Num(); Hit++)
        {
            if (OutHits[Hit].GetActor() && Cast<AAmmoBox>(OutHits[Hit].GetActor()))
            {
                AAmmoBox* Box = Cast<AAmmoBox>(OutHits[Hit].GetActor());
                
                // Try to use the behavior component first
                UBotBehaviorComponent* BehaviorComponent = BotController->GetBotBehaviorComponent();
                if (BehaviorComponent)
                {
                    BehaviorComponent->SetAmmoBox(Box);
                }
                else
                {
                    // Fall back to the controller method for backward compatibility
                    BotController->SetAmmoBox(Box);
                }

                return EBTNodeResult::Succeeded;
            }
        }
    }

    return EBTNodeResult::Failed;
}