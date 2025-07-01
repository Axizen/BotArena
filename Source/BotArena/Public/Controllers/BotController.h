// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BotController.generated.h"

// Forward declarations
class UBotPerceptionComponent;
class UBotBehaviorComponent;
class UBotPathFollowingComponent;
class UAIPerceptionComponent;
class UAIPerceptionStimuliSourceComponent;
class AAmmoBox;

UCLASS()
class BOTARENA_API ABotController : public AAIController
{
    GENERATED_BODY()

public:
    ABotController();

    // Called when the game starts
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    
    // Called when possessing a pawn
    virtual void OnPossess(APawn* InPawn) override;
    
    // Called when unpossessing a pawn
    virtual void OnUnPossess() override;
    
    // Delegate methods to components for backward compatibility
    UFUNCTION(BlueprintCallable, Category = "BotArena")
    FVector GetSelectedTargetLocation() const;
    
    UFUNCTION(BlueprintCallable, Category = "BotArena")
    void SetMoveToLocation(const FVector& Location);
    
    UFUNCTION(BlueprintCallable, Category = "BotArena")
    void SetAmmoBox(class AAmmoBox* AmmoBox);
    
    UFUNCTION(BlueprintPure, Category = "BotArena")
    class UBehaviorTree* GetCurrentTree();
    
    UFUNCTION(BlueprintPure, Category = "BotArena")
    AActor* GetSelectedTarget() const;
    
    UFUNCTION(BlueprintCallable, Category = "BotArena")
    void InitiateRetreat();
    
    UFUNCTION(BlueprintCallable, Category = "BotArena")
    void SetCollectAmmoStatus(bool NewStatus);

protected:
    // Perception component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UBotPerceptionComponent* BotPerceptionComponent;
    
    // Behavior component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UBotBehaviorComponent* BotBehaviorComponent;
    
    // The custom path following component (kept for compatibility)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UBotPathFollowingComponent* BotPathFollowingComp;
    
    // The Perception component for this controller (kept for compatibility)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UAIPerceptionComponent* PerceptionComp;
    
    // Stimuli source component (kept for compatibility)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UAIPerceptionStimuliSourceComponent* StimuliSourceComp;

public:
    // Getters for components
    UFUNCTION(BlueprintPure, Category = "Components")
    class UBotPerceptionComponent* GetBotPerceptionComponent() const { return BotPerceptionComponent; }
    
    UFUNCTION(BlueprintPure, Category = "Components")
    class UBotBehaviorComponent* GetBotBehaviorComponent() const { return BotBehaviorComponent; }
};