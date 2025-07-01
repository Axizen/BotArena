// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BotCoreComponent.h"
#include "BotBehaviorComponent.generated.h"

UCLASS(ClassGroup=(BotArena), meta=(BlueprintSpawnableComponent))
class BOTARENA_API UBotBehaviorComponent : public UBotCoreComponent
{
    GENERATED_BODY()

public:
    UBotBehaviorComponent();

    // Called when the game starts
    virtual void BeginPlay() override;
    
    // Initialize behavior tree
    UFUNCTION(BlueprintCallable, Category = "Behavior")
    void InitializeBehavior();
    
    // Set move to location
    UFUNCTION(BlueprintCallable, Category = "Behavior")
    void SetMoveToLocation(const FVector& Location);
    
    // Set ammo box
    UFUNCTION(BlueprintCallable, Category = "Behavior")
    void SetAmmoBox(class AAmmoBox* AmmoBox);
    
    // Initiate retreat
    UFUNCTION(BlueprintCallable, Category = "Behavior")
    void InitiateRetreat();
    
    // Set collect ammo status
    UFUNCTION(BlueprintCallable, Category = "Behavior")
    void SetCollectAmmoStatus(bool NewStatus);
    
    // Get behavior tree
    UFUNCTION(BlueprintPure, Category = "Behavior")
    class UBehaviorTree* GetBehaviorTree() const { return BTAsset; }

protected:
    // Behavior tree asset
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Behavior")
    class UBehaviorTree* BTAsset;
    
    // Blackboard key names
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Behavior|Blackboard")
    FName BlackboardKey_MoveLocation;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Behavior|Blackboard")
    FName BlackboardKey_SelectedTarget;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Behavior|Blackboard")
    FName BlackboardKey_ShouldRetreat;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Behavior|Blackboard")
    FName BlackboardKey_CollectAmmo;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Behavior|Blackboard")
    FName BlackboardKey_AmmoBox;
};