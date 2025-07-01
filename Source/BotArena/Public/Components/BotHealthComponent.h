// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BotCoreComponent.h"
#include "BotHealthComponent.generated.h"

// Delegate for health changed events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, Health, float, HealthDelta);
// Delegate for death events
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS(ClassGroup=(BotArena), meta=(BlueprintSpawnableComponent))
class BOTARENA_API UBotHealthComponent : public UBotCoreComponent
{
    GENERATED_BODY()

public:
    UBotHealthComponent();

    // Called when the game starts
    virtual void BeginPlay() override;

    // Handle damage taken
    UFUNCTION(BlueprintCallable, Category = "Health")
    float HandleDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
    
    // Check if the bot is alive
    UFUNCTION(BlueprintPure, Category = "Health")
    bool IsAlive() const;
    
    // Get current health
    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealth() const { return Health; }
    
    // Get max health
    UFUNCTION(BlueprintPure, Category = "Health")
    float GetMaxHealth() const { return MaxHealth; }
    
    // Check if should retreat
    UFUNCTION(BlueprintPure, Category = "Health")
    bool ShouldRetreat() const;
    
    // Health changed delegate
    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnHealthChangedSignature OnHealthChanged;
    
    // Death delegate
    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnDeathSignature OnDeath;

protected:
    // Current health
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float Health;
    
    // Maximum health
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float MaxHealth;
    
    // Health percentage that triggers retreat
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta=(ClampMin="0.0", ClampMax="1.0"))
    float RetreatHealthPercentage;
    
    // Handle death effects
    UFUNCTION(BlueprintCallable, Category = "Health")
    void HandleDeath();
    
    // Delay before destroying the actor after death
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float DestroyActorDelay;
};