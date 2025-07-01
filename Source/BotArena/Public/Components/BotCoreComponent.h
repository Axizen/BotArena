// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BotCoreComponent.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup=(BotArena), meta=(BlueprintSpawnableComponent))
class BOTARENA_API UBotCoreComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBotCoreComponent();

    // Called when the game starts
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    // Initialize the component with its owner
    UFUNCTION(BlueprintCallable, Category = "Bot Core")
    virtual void InitializeComponent();
    
    // Get the AICharacter owner
    UFUNCTION(BlueprintCallable, Category = "Bot Core")
    class AAICharacter* GetAICharacterOwner() const;
    
    // Get the BotController owner
    UFUNCTION(BlueprintCallable, Category = "Bot Core")
    class ABotController* GetBotController() const;
    
    // Cached references to owners
    UPROPERTY(Transient)
    class AAICharacter* AICharacterOwner;
    
    UPROPERTY(Transient)
    class ABotController* BotControllerOwner;
};