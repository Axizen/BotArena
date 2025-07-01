// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/BotMovementComponent.h"
#include "Characters/AICharacter.h"
#include "Controllers/BotController.h"
#include "Components/BotPathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBotMovementComponent::UBotMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Default values
    MeshCrouchAdjustLocation = FVector(0, 0, -70);
}

void UBotMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize movement when the component begins play
    InitializeMovement();
}

void UBotMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Adjust mesh for crouch if needed
    AdjustMeshForCrouch();
}

void UBotMovementComponent::InitializeMovement()
{
    ABotController* BotController = GetBotController();
    if (!BotController)
    {
        return;
    }
    
    // Get the path following component from the controller
    BotPathFollowingComp = Cast<UBotPathFollowingComponent>(BotController->GetPathFollowingComponent());
    if (!BotPathFollowingComp)
    {
        // If the controller doesn't have a BotPathFollowingComponent, it might be using the default one
        // We could log a warning here, but it's not necessarily an error
        UE_LOG(LogTemp, Warning, TEXT("BotController does not have a BotPathFollowingComponent"));
    }
}

void UBotMovementComponent::AdjustMeshForCrouch()
{
    AAICharacter* Character = GetAICharacterOwner();
    if (!Character)
    {
        return;
    }
    
    UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
    if (MovementComp && MovementComp->IsCrouching())
    {
        // Adjust the mesh height to avoid having half of the mesh under the ground
        Character->GetMesh()->SetRelativeLocation(MeshCrouchAdjustLocation, false, nullptr, ETeleportType::TeleportPhysics);
    }
}