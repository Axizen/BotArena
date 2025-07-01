// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BotTeamComponent.h"
#include "AICharacter.generated.h"

// Forward declarations
class UBotHealthComponent;
class UBotWeaponComponent;
class UBotMovementComponent;

UCLASS()
class BOTARENA_API AAICharacter : public ACharacter
{
    GENERATED_BODY()
    
public:
    // Sets default values for this character's properties
    AAICharacter();

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    
    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    // Take damage override
    virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
    
    // Construction script
    virtual void OnConstruction(const FTransform& Transform) override;
    
    // Blueprint event for team assignment
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "BotArena")
    void AssignTeam(ETeam NewTeam);

protected:
    // Health component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBotHealthComponent* HealthComponent;
    
    // Weapon component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBotWeaponComponent* WeaponComponent;
    
    // Team component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBotTeamComponent* TeamComponent;
    
    // Movement component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBotMovementComponent* BotMovementComponent;

public:
    // Getters for components
    UFUNCTION(BlueprintPure, Category = "Components")
    UBotHealthComponent* GetHealthComponent() const { return HealthComponent; }
    
    UFUNCTION(BlueprintPure, Category = "Components")
    UBotWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }
    
    UFUNCTION(BlueprintPure, Category = "Components")
    UBotTeamComponent* GetTeamComponent() const { return TeamComponent; }
    
    UFUNCTION(BlueprintPure, Category = "Components")
    UBotMovementComponent* GetBotMovementComponent() const { return BotMovementComponent; }
    
    // Delegate methods to components for backward compatibility
    UFUNCTION(BlueprintCallable, Category = "BotArena")
    void FireWeapon();
    
    UFUNCTION(BlueprintPure, Category = "BotArena")
    bool IsAlive() const;
    
    UFUNCTION(BlueprintCallable, Category = "BotArena")
    void AddAmmo(int32 Ammo);
    
    UFUNCTION(BlueprintPure, Category = "BotArena")
    int32 GetCurrentAmmo() const;
    
    UFUNCTION(BlueprintPure, Category = "BotArena")
    bool LowOnAmmo() const;
    
    UFUNCTION(BlueprintPure, Category = "BotArena")
    bool IsFriendly(const AAICharacter* OtherCharacter) const;
    
    UFUNCTION(BlueprintPure, Category = "BotArena")
    bool IsHostile(const AAICharacter* OtherCharacter) const;
    
    UFUNCTION(BlueprintPure, Category = "BotArena")
    ETeam GetTeam() const;
    
    UFUNCTION(BlueprintPure, Category = "BotArena")
    bool SameTeam(const AAICharacter* OtherCharacter) const;
};