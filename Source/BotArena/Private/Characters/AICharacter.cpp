// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AICharacter.h"
#include "Components/BotHealthComponent.h"
#include "Components/BotWeaponComponent.h"
#include "Components/BotTeamComponent.h"
#include "Components/BotMovementComponent.h"
#include "Controllers/BotController.h"

// Sets default values
AAICharacter::AAICharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Create components
    HealthComponent = CreateDefaultSubobject<UBotHealthComponent>(TEXT("HealthComponent"));
    WeaponComponent = CreateDefaultSubobject<UBotWeaponComponent>(TEXT("WeaponComponent"));
    TeamComponent = CreateDefaultSubobject<UBotTeamComponent>(TEXT("TeamComponent"));
    BotMovementComponent = CreateDefaultSubobject<UBotMovementComponent>(TEXT("BotMovementComponent"));

    // Set auto possess AI
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AAICharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AAICharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // Delegate to health component
    if (HealthComponent)
    {
        return HealthComponent->HandleDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
    }
    
    return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void AAICharacter::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    
    // Initialize health to max health
    if (HealthComponent)
    {
        HealthComponent->BeginPlay(); // Force initialization
    }
}

// Delegate methods implementation
void AAICharacter::FireWeapon()
{
    if (WeaponComponent)
    {
        WeaponComponent->FireWeapon();
    }
}

bool AAICharacter::IsAlive() const
{
    return HealthComponent ? HealthComponent->IsAlive() : false;
}

void AAICharacter::AddAmmo(int32 Ammo)
{
    if (WeaponComponent)
    {
        WeaponComponent->AddAmmo(Ammo);
    }
}

int32 AAICharacter::GetCurrentAmmo() const
{
    return WeaponComponent ? WeaponComponent->GetCurrentAmmo() : 0;
}

bool AAICharacter::LowOnAmmo() const
{
    return WeaponComponent ? WeaponComponent->LowOnAmmo() : true;
}

bool AAICharacter::IsFriendly(const AAICharacter* OtherCharacter) const
{
    return TeamComponent ? TeamComponent->IsFriendly(OtherCharacter) : false;
}

bool AAICharacter::IsHostile(const AAICharacter* OtherCharacter) const
{
    return TeamComponent ? TeamComponent->IsHostile(OtherCharacter) : true;
}

ETeam AAICharacter::GetTeam() const
{
    return TeamComponent ? TeamComponent->GetTeam() : ETeam::E_Team1;
}

bool AAICharacter::SameTeam(const AAICharacter* OtherCharacter) const
{
    return IsFriendly(OtherCharacter);
}