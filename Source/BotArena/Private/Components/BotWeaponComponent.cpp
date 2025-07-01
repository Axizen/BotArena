// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/BotWeaponComponent.h"
#include "Characters/AICharacter.h"
#include "Controllers/BotController.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "MiscClasses/Projectile.h"
#include "Components/BotHealthComponent.h"
#include "Components/BotTeamComponent.h"
#include "LogBotArena.h"
#include "Utils/BotArenaUtils.h"

UBotWeaponComponent::UBotWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Create weapon mesh
    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    
    // Create weapon fire effect
    WeaponFireFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("WeaponFireFX"));
    WeaponFireFX->SetupAttachment(WeaponMesh);
    WeaponFireFX->SetAutoActivate(false);
    
    // Default values
    CurrentAmmo = 30;
    FireDelay = 0.35f;
    DeactivateParticleDelay = 0.2f;
    LastFireWeaponTime = 0.0f;
    LowAmmoThreshold = 5;
}

void UBotWeaponComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Attach weapon to character mesh if we're owned by an AICharacter
    AAICharacter* Character = GetAICharacterOwner();
    if (Character && WeaponMesh)
    {
        WeaponMesh->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("WeaponSocket"));
        
        if (WeaponFireFX)
        {
            WeaponFireFX->AttachToComponent(WeaponMesh, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
            WeaponFireFX->SetWorldLocation(WeaponMesh->GetSocketLocation("BulletSocket"));
        }
    }
}

void UBotWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Update last fire time
    LastFireWeaponTime += DeltaTime;
    
    // Deactivate particle effect after delay
    if (LastFireWeaponTime >= DeactivateParticleDelay)
    {
        DeactivateFireWeaponParticle();
    }
}

void UBotWeaponComponent::FireWeapon()
{
    if (!IsValid(GetOwner()))
    {
        UE_LOG(LogBotArena, Warning, TEXT("FireWeapon: Invalid owner"));
        return;
    }
    
    if (!WeaponMesh)
    {
        UE_LOG(LogBotArena, Warning, TEXT("%s: WeaponMesh is null"), *GetNameSafe(GetOwner()));
        return;
    }
    
    if (!CanFireWeapon())
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: Cannot fire weapon"), *GetNameSafe(GetOwner()));
        return;
    }
    
    if (!CanSeeSelectedTarget())
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: Cannot see selected target"), *GetNameSafe(GetOwner()));
        return;
    }
    
    ABotController* BotController = GetBotController();
    if (!BotController)
    {
        UE_LOG(LogBotArena, Warning, TEXT("%s: Missing BotController"), *GetNameSafe(GetOwner()));
        return;
    }
    
    AActor* SelectedTarget = BotController->GetSelectedTarget();
    if (!SelectedTarget)
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: No selected target"), *GetNameSafe(GetOwner()));
        return;
    }
    
    // Get weapon muzzle location
    FVector WeaponMuzzle = WeaponMesh->GetSocketLocation(FName("BulletSocket"));
    FVector BulletEndLocation = BotController->GetSelectedTargetLocation();
    
    // Spawn projectile
    if (!ProjectileBP)
    {
        UE_LOG(LogBotArena, Warning, TEXT("%s: ProjectileBP is null"), *GetNameSafe(GetOwner()));
    }
    else if (!WeaponFireFX)
    {
        UE_LOG(LogBotArena, Warning, TEXT("%s: WeaponFireFX is null"), *GetNameSafe(GetOwner()));
    }
    else
    {
        AProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileBP, FTransform(WeaponMuzzle), FActorSpawnParameters());
        if (SpawnedProjectile)
        {
            SpawnedProjectile->AdjustVelocity(BulletEndLocation);
            UE_LOG(LogBotArena, Verbose, TEXT("%s: Projectile spawned successfully"), *GetNameSafe(GetOwner()));
        }
        else
        {
            UE_LOG(LogBotArena, Warning, TEXT("%s: Failed to spawn projectile"), *GetNameSafe(GetOwner()));
        }
        
        // Activate the particle and adjust its end point
        WeaponFireFX->Activate();
        WeaponFireFX->SetBeamEndPoint(0, BulletEndLocation);
    }
    
    // Reduce ammo and reset fire timer
    CurrentAmmo--;
    LastFireWeaponTime = 0.0f;
    
    // Broadcast weapon fired event
    OnWeaponFired.Broadcast();
    OnAmmoChanged.Broadcast(CurrentAmmo);
    
    UE_LOG(LogBotArena, Log, TEXT("%s: Weapon fired, %d ammo remaining"), 
           *GetNameSafe(GetOwner()), CurrentAmmo);
}

bool UBotWeaponComponent::CanFireWeapon() const
{
    if (!IsValid(GetOwner()))
    {
        UE_LOG(LogBotArena, Warning, TEXT("CanFireWeapon: Invalid owner"));
        return false;
    }
    
    // Check if we have ammo
    if (CurrentAmmo <= 0)
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: Cannot fire weapon - no ammo"), *GetNameSafe(GetOwner()));
        return false;
    }
    
    // Check if enough time has passed since last fire
    if (LastFireWeaponTime < FireDelay)
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: Cannot fire weapon - on cooldown"), *GetNameSafe(GetOwner()));
        return false;
    }
    
    // Check if the bot is alive
    UBotHealthComponent* HealthComp = FBotArenaUtils::GetComponentSafe<UBotHealthComponent>(GetOwner(), TEXT("HealthComponent"));
    bool IsAlive = HealthComp ? HealthComp->IsAlive() : false;
    
    if (!IsAlive)
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: Cannot fire weapon - bot is not alive"), *GetNameSafe(GetOwner()));
        return false;
    }
    
    return true;
}

bool UBotWeaponComponent::CanSeeSelectedTarget() const
{
    if (!IsValid(GetOwner()))
    {
        UE_LOG(LogBotArena, Warning, TEXT("CanSeeSelectedTarget: Invalid owner"));
        return false;
    }
    
    ABotController* BotController = GetBotController();
    if (!BotController)
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: No BotController"), *GetNameSafe(GetOwner()));
        return false;
    }
    
    AActor* SelectedTarget = BotController->GetSelectedTarget();
    if (!SelectedTarget)
    {
        UE_LOG(LogBotArena, Verbose, TEXT("%s: No selected target"), *GetNameSafe(GetOwner()));
        return false;
    }
    
    // Get target location
    FVector TargetLocation = BotController->GetSelectedTargetLocation();
    FVector StartLocation = GetOwner()->GetActorLocation();
    
    // Perform line trace to check if we can see the target
    FCollisionObjectQueryParams CollisionObjectQueryParams = FCollisionObjectQueryParams(ECC_WorldDynamic);
    CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
    CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
    
    FHitResult HitResult;
    FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams(FName("BotLineTrace"));
    CollisionQueryParams.AddIgnoredActor(GetOwner());
    
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogBotArena, Warning, TEXT("%s: Invalid World"), *GetNameSafe(GetOwner()));
        return false;
    }
    
    if (World->LineTraceSingleByObjectType(HitResult, StartLocation, TargetLocation, CollisionObjectQueryParams, CollisionQueryParams))
    {
        AAICharacter* Character = Cast<AAICharacter>(HitResult.GetActor());
        if (!Character)
        {
            // Hit something that's not a character
            UE_LOG(LogBotArena, Verbose, TEXT("%s: Line trace hit non-character: %s"), 
                   *GetNameSafe(GetOwner()), *GetNameSafe(HitResult.GetActor()));
            return false;
        }
        
        AAICharacter* OwnerCharacter = Cast<AAICharacter>(GetOwner());
        if (!OwnerCharacter)
        {
            UE_LOG(LogBotArena, Warning, TEXT("%s: Owner is not an AICharacter"), *GetNameSafe(GetOwner()));
            return false;
        }
        
        bool bIsHostile = OwnerCharacter->IsHostile(Character);
        UE_LOG(LogBotArena, Verbose, TEXT("%s: Can see character %s, hostile: %s"), 
               *GetNameSafe(GetOwner()), *GetNameSafe(Character), bIsHostile ? TEXT("true") : TEXT("false"));
        return bIsHostile;
    }
    
    UE_LOG(LogBotArena, Verbose, TEXT("%s: Line trace did not hit target"), *GetNameSafe(GetOwner()));
    return false;
}

void UBotWeaponComponent::AddAmmo(int32 AmmoAmount)
{
    if (AmmoAmount <= 0)
    {
        UE_LOG(LogBotArena, Warning, TEXT("%s: Attempted to add invalid ammo amount: %d"), 
               *GetNameSafe(GetOwner()), AmmoAmount);
        return;
    }
    
    // Define a reasonable maximum ammo addition to prevent unrealistic values
    const int32 MaxAmmoAddition = 100;
    if (AmmoAmount > MaxAmmoAddition)
    {
        UE_LOG(LogBotArena, Warning, TEXT("%s: Ammo addition exceeds maximum: %d > %d"), 
               *GetNameSafe(GetOwner()), AmmoAmount, MaxAmmoAddition);
        AmmoAmount = MaxAmmoAddition;
    }
    
    int32 OldAmmo = CurrentAmmo;
    
    // Define a maximum ammo capacity
    const int32 MaxAmmoCapacity = 999;
    CurrentAmmo = FMath::Clamp(CurrentAmmo + AmmoAmount, 0, MaxAmmoCapacity);
    
    // Broadcast ammo changed event if ammo actually changed
    if (OldAmmo != CurrentAmmo)
    {
        UE_LOG(LogBotArena, Log, TEXT("%s: Ammo changed from %d to %d"), 
               *GetNameSafe(GetOwner()), OldAmmo, CurrentAmmo);
        OnAmmoChanged.Broadcast(CurrentAmmo);
    }
}

bool UBotWeaponComponent::LowOnAmmo() const
{
    return CurrentAmmo <= LowAmmoThreshold;
}

void UBotWeaponComponent::DeactivateFireWeaponParticle()
{
    if (WeaponFireFX)
    {
        WeaponFireFX->SetBeamEndPoint(0, WeaponFireFX->GetComponentLocation());
        WeaponFireFX->Deactivate();
    }
}