// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BotCoreComponent.h"
#include "HAL/CriticalSection.h"
#include "BotPerceptionComponent.generated.h"

// Delegate for target selected events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetSelectedSignature, AActor*, SelectedTarget);

UCLASS(ClassGroup=(BotArena), meta=(BlueprintSpawnableComponent))
class BOTARENA_API UBotPerceptionComponent : public UBotCoreComponent
{
    GENERATED_BODY()

public:
    UBotPerceptionComponent();

    // Called when the game starts
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    // Initialize perception
    UFUNCTION(BlueprintCallable, Category = "Perception")
    void InitializePerception();
    
    // Select target from sensed actors
    UFUNCTION(BlueprintCallable, Category = "Perception")
    void SelectTarget(const TArray<AActor*>& TargetList);
    
    // Get selected target
    UFUNCTION(BlueprintPure, Category = "Perception")
    AActor* GetSelectedTarget() const;
    
    // Get selected target location
    UFUNCTION(BlueprintPure, Category = "Perception")
    FVector GetSelectedTargetLocation() const;
    
    // Target selected delegate
    UPROPERTY(BlueprintAssignable, Category = "Perception")
    FOnTargetSelectedSignature OnTargetSelected;

protected:
    // Perception component reference
    UPROPERTY(Transient)
    class UAIPerceptionComponent* PerceptionComp;
    
    // Time since target selection
    float TimeSinceTargetSelection;
    
    // Target selection interval
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Perception")
    float SelectTargetInterval;
    
    // Target rotation speed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
    float SelectTargetRotationSpeed;
    
    // Perception updated handler
    UFUNCTION()
    void OnPerceptionUpdated(const TArray<AActor*>& SensedActors);
    
    // Thread safety for perception updates
    FCriticalSection PerceptionLock;
    TArray<AActor*> PendingSensedActors;
    bool bHasPendingPerceptionUpdate = false;
    
    // Debug visualization
    UFUNCTION()
    void DebugDrawPerception(float DeltaTime);
};