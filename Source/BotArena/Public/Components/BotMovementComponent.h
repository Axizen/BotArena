// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BotCoreComponent.h"
#include "BotMovementComponent.generated.h"

UCLASS(ClassGroup=(BotArena), meta=(BlueprintSpawnableComponent))
class BOTARENA_API UBotMovementComponent : public UBotCoreComponent
{
    GENERATED_BODY()

public:
    UBotMovementComponent();

    // Called when the game starts
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    // Initialize movement
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void InitializeMovement();
    
    // Get path following component
    UFUNCTION(BlueprintPure, Category = "Movement")
    class UBotPathFollowingComponent* GetPathFollowingComponent() const { return BotPathFollowingComp; }

protected:
    // Path following component
    UPROPERTY(Transient)
    class UBotPathFollowingComponent* BotPathFollowingComp;
    
    // Mesh crouch adjust location
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector MeshCrouchAdjustLocation;
    
    // Adjust mesh for crouch
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void AdjustMeshForCrouch();
};