// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/BotCoreComponent.h"
#include "Characters/AICharacter.h"
#include "Controllers/BotController.h"

UBotCoreComponent::UBotCoreComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    AICharacterOwner = nullptr;
    BotControllerOwner = nullptr;
}

void UBotCoreComponent::BeginPlay()
{
    Super::BeginPlay();
    InitializeComponent();
}

void UBotCoreComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBotCoreComponent::InitializeComponent()
{
    AICharacterOwner = Cast<AAICharacter>(GetOwner());
    
    if (AICharacterOwner)
    {
        BotControllerOwner = Cast<ABotController>(AICharacterOwner->GetController());
    }
    else
    {
        BotControllerOwner = Cast<ABotController>(GetOwner());
        if (BotControllerOwner)
        {
            AICharacterOwner = Cast<AAICharacter>(BotControllerOwner->GetPawn());
        }
    }
}

AAICharacter* UBotCoreComponent::GetAICharacterOwner() const
{
    return AICharacterOwner;
}

ABotController* UBotCoreComponent::GetBotController() const
{
    return BotControllerOwner;
}