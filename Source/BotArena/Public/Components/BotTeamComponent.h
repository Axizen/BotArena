// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BotCoreComponent.h"
#include "BotTeamComponent.generated.h"

// Enum for team types
UENUM(BlueprintType)
enum class ETeam : uint8
{
    E_Team1 UMETA(DisplayName="Team 1"),
    E_Team2 UMETA(DisplayName="Team 2"),
    E_Team3 UMETA(DisplayName="Team 3")
};

// Delegate for team changed events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamChangedSignature, ETeam, NewTeam);

UCLASS(ClassGroup=(BotArena), meta=(BlueprintSpawnableComponent))
class BOTARENA_API UBotTeamComponent : public UBotCoreComponent
{
    GENERATED_BODY()

public:
    UBotTeamComponent();

    // Called when the game starts
    virtual void BeginPlay() override;
    
    // Set team
    UFUNCTION(BlueprintCallable, Category = "Team")
    void SetTeam(ETeam NewTeam);
    
    // Get team
    UFUNCTION(BlueprintPure, Category = "Team")
    ETeam GetTeam() const { return Team; }
    
    // Check if friendly
    UFUNCTION(BlueprintPure, Category = "Team")
    bool IsFriendly(const AAICharacter* OtherCharacter) const;
    
    // Check if hostile
    UFUNCTION(BlueprintPure, Category = "Team")
    bool IsHostile(const AAICharacter* OtherCharacter) const;
    
    // Team changed delegate
    UPROPERTY(BlueprintAssignable, Category = "Team")
    FOnTeamChangedSignature OnTeamChanged;

protected:
    // Current team
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
    ETeam Team;
};