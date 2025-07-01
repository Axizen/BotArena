// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BotPathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Environment/NavArea_Crouch.h"
#include "LogBotArena.h"

void UBotPathFollowingComponent::SetMoveSegment(int32 SegmentStartIndex)
{
	Super::SetMoveSegment(SegmentStartIndex);

	if (!CharacterMovementComp)
	{
		UE_LOG(LogBotArena, Warning, TEXT("SetMoveSegment: CharacterMovementComp is null"));
		return;
	}

	if (!Path.IsValid())
	{
		UE_LOG(LogBotArena, Warning, TEXT("SetMoveSegment: Path is invalid"));
		return;
	}

	const TArray<FNavPathPoint>& PathPoints = Path->GetPathPoints();
	if (PathPoints.Num() <= SegmentStartIndex)
	{
		UE_LOG(LogBotArena, Warning, TEXT("SetMoveSegment: Invalid segment index %d for path with %d points"), 
			SegmentStartIndex, PathPoints.Num());
		return;
	}

	//Get the nav point and check if it has a crouch flag
	const FNavPathPoint& SegmentStart = PathPoints[SegmentStartIndex];
	if (FNavAreaHelper::HasCrouchFlag(SegmentStart))
	{
		CharacterMovementComp->bWantsToCrouch = true;
		CharacterMovementComp->Crouch();
		UE_LOG(LogBotArena, Verbose, TEXT("SetMoveSegment: Character attempting to crouch"));
	}
	else
	{
		CharacterMovementComp->bWantsToCrouch = false;
	}
}

void UBotPathFollowingComponent::SetMovementComponent(UNavMovementComponent* MoveComp)
{
	Super::SetMovementComponent(MoveComp);
	
	if (!MoveComp)
	{
		UE_LOG(LogBotArena, Warning, TEXT("SetMovementComponent: Received null MoveComp"));
		CharacterMovementComp = nullptr;
		return;
	}
	
	CharacterMovementComp = Cast<UCharacterMovementComponent>(MoveComp);
	
	if (!CharacterMovementComp)
	{
		UE_LOG(LogBotArena, Warning, TEXT("SetMovementComponent: MoveComp is not a CharacterMovementComponent"));
	}
	else
	{
		UE_LOG(LogBotArena, Verbose, TEXT("SetMovementComponent: Successfully set CharacterMovementComp"));
	}
}
