// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BotCoreComponent.h"
#include "BotWeaponComponent.generated.h"

// Delegate for weapon fired events
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFiredSignature);
// Delegate for ammo changed events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoChangedSignature, int32, NewAmmo);

UCLASS(ClassGroup=(BotArena), meta=(BlueprintSpawnableComponent))
class BOTARENA_API UBotWeaponComponent : public UBotCoreComponent
{
    GENERATED_BODY()

public:
    UBotWeaponComponent();

    // Called when the game starts
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    // Fire the weapon
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void FireWeapon();
    
    // Check if can fire weapon
    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool CanFireWeapon() const;
    
    // Check if can see target
    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool CanSeeSelectedTarget() const;
    
    // Add ammo
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void AddAmmo(int32 AmmoAmount);
    
    // Get current ammo
    UFUNCTION(BlueprintPure, Category = "Weapon")
    int32 GetCurrentAmmo() const { return CurrentAmmo; }
    
    // Check if low on ammo
    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool LowOnAmmo() const;
    
    // Weapon fired delegate
    UPROPERTY(BlueprintAssignable, Category = "Weapon")
    FOnWeaponFiredSignature OnWeaponFired;
    
    // Ammo changed delegate
    UPROPERTY(BlueprintAssignable, Category = "Weapon")
    FOnAmmoChangedSignature OnAmmoChanged;

protected:
    // Weapon static mesh
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    class UStaticMeshComponent* WeaponMesh;
    
    // Weapon fire effect
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    class UParticleSystemComponent* WeaponFireFX;
    
    // Current ammo
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 CurrentAmmo;
    
    // Projectile class
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<class AProjectile> ProjectileBP;
    
    // Fire delay
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float FireDelay;
    
    // Deactivate particle delay
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float DeactivateParticleDelay;
    
    // Time since last fire
    float LastFireWeaponTime;
    
    // Deactivate fire weapon particle
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void DeactivateFireWeaponParticle();
    
    // Low ammo threshold
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 LowAmmoThreshold;
};