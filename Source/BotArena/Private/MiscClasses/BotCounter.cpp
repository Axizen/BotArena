// Fill out your copyright notice in the Description page of Project Settings.


#include "MiscClasses/BotCounter.h"
#include "LogBotArena.h"

// Sets default values
ABotCounter::ABotCounter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize member variables
	Blue_Bots = 0;
	Red_Bots = 0;
}

// Called when the game starts or when spawned
void ABotCounter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABotCounter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABotCounter::OnBotSpawn(ETeam BotTeam)
{
	switch (BotTeam)
	{
	case ETeam::E_Team1:
		Blue_Bots++;
		break;
	case ETeam::E_Team2:
		Red_Bots++;
		break;
	case ETeam::E_Team3:
		// Handle Team3 or log a warning
		UE_LOG(LogBotArena, Warning, TEXT("OnBotSpawn: Team3 not handled in counter"));
		break;
	default:
		UE_LOG(LogBotArena, Error, TEXT("OnBotSpawn: Unknown team enum value: %d"), (int32)BotTeam);
		break;
	}
}

void ABotCounter::OnBotDeath(ETeam BotTeam)
{
	switch (BotTeam)
	{
	case ETeam::E_Team1:
		Blue_Bots--;
		break;
	case ETeam::E_Team2:
		Red_Bots--;
		break;
	case ETeam::E_Team3:
		// Handle Team3 or log a warning
		UE_LOG(LogBotArena, Warning, TEXT("OnBotDeath: Team3 not handled in counter"));
		break;
	default:
		UE_LOG(LogBotArena, Error, TEXT("OnBotDeath: Unknown team enum value: %d"), (int32)BotTeam);
		break;
	}
}

