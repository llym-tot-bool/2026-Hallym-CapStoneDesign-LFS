// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SoulslikeGameMode.generated.h"

class ABoss;

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class ASoulslikeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	ASoulslikeGameMode();

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void RegisterSpawnedBoss(ABoss* InBoss);

	UFUNCTION(BlueprintPure, Category = "Boss")
	ABoss* GetCurrentBossSafe() const;

	UFUNCTION(BlueprintCallable, Category = "Boss")
	bool TryGetCurrentBoss(ABoss*& OutBoss) const;

private:
	UPROPERTY()
	TWeakObjectPtr<ABoss> CurrentBoss;
};



