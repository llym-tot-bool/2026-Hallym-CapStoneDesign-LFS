// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class SOULSLIKE_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawner();

protected:
	virtual void BeginPlay() override;

	// Enemy class to spawn (BP/C++)
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AActor> EnemyClass;

	// Spawn probability on BeginPlay (0.0 = never, 1.0 = always)
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float SpawnChance = 0.6f;

	// Added to SpawnChance whenever a new map/world is loaded after the first load.
	UPROPERTY(EditAnywhere, Category = "Spawn|Progression", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float SpawnChanceIncreasePerMapLoad = 0.2f;

	// Final capped chance after map-load bonus is applied.
	UPROPERTY(EditAnywhere, Category = "Spawn|Progression", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MaxSpawnChance = 1.0f;

private:
	void TrySpawnEnemyOnce();
};
