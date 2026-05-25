// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulslikeGameMode.h"

#include "Boss.h"
#include "Kismet/GameplayStatics.h"

ASoulslikeGameMode::ASoulslikeGameMode()
{
	// stub
}

void ASoulslikeGameMode::RegisterSpawnedBoss(ABoss* InBoss)
{
	if (!IsValid(InBoss))
	{
		return;
	}

	CurrentBoss = InBoss;
}

ABoss* ASoulslikeGameMode::GetCurrentBossSafe() const
{
	if (CurrentBoss.IsValid())
	{
		return CurrentBoss.Get();
	}

	AActor* FoundBoss = UGameplayStatics::GetActorOfClass(GetWorld(), ABoss::StaticClass());
	return Cast<ABoss>(FoundBoss);
}

bool ASoulslikeGameMode::TryGetCurrentBoss(ABoss*& OutBoss) const
{
	OutBoss = GetCurrentBossSafe();
	return IsValid(OutBoss);
}
