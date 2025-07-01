// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/BotTeamComponent.h"
#include "Characters/AICharacter.h"

UBotTeamComponent::UBotTeamComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    // Default team
    Team = ETeam::E_Team1;
}

void UBotTeamComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UBotTeamComponent::SetTeam(ETeam NewTeam)
{
    if (Team != NewTeam)
    {
        Team = NewTeam;
        
        // Broadcast team changed event
        OnTeamChanged.Broadcast(Team);
        
        // Notify the AICharacter if it has a blueprint implementation
        AAICharacter* Character = GetAICharacterOwner();
        if (Character)
        {
            Character->AssignTeam(Team);
        }
    }
}

bool UBotTeamComponent::IsFriendly(const AAICharacter* OtherCharacter) const
{
    if (!OtherCharacter)
    {
        return false;
    }
    
    // Get the other character's team component
    const UBotTeamComponent* OtherTeamComponent = OtherCharacter->FindComponentByClass<UBotTeamComponent>();
    if (!OtherTeamComponent)
    {
        return false;
    }
    
    // Compare teams
    return Team == OtherTeamComponent->GetTeam();
}

bool UBotTeamComponent::IsHostile(const AAICharacter* OtherCharacter) const
{
    return !IsFriendly(OtherCharacter);
}