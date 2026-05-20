#include "EnemySpawner.h"
#include "Engine/World.h"
#include "enemy_mobs.h"

namespace
{
	const UWorld* LastProcessedWorld = nullptr;
	float SpawnChanceBonusFromMapLoads = 0.0f;
	bool bHasSeenFirstWorld = false;
}

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* CurrentWorld = GetWorld();
	if (CurrentWorld && LastProcessedWorld != CurrentWorld)
	{
		if (bHasSeenFirstWorld)
		{
			const AEnemySpawner* DefaultSpawner = GetDefault<AEnemySpawner>();
			SpawnChanceBonusFromMapLoads += DefaultSpawner ? DefaultSpawner->SpawnChanceIncreasePerMapLoad : 0.0f;
		}
		else
		{
			bHasSeenFirstWorld = true;
		}

		LastProcessedWorld = CurrentWorld;
	}

	TrySpawnEnemyOnce();
}

void AEnemySpawner::TrySpawnEnemyOnce()
{
	if (!EnemyClass || !GetWorld())
	{
		return;
	}

	const float EffectiveSpawnChance = FMath::Clamp(SpawnChance + SpawnChanceBonusFromMapLoads, 0.0f, MaxSpawnChance);
	const float Roll = FMath::FRand();
	if (Roll > EffectiveSpawnChance)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[EnemySpawner] Spawn skipped. Roll=%.3f Chance=%.3f Spawner=%s"), Roll, EffectiveSpawnChance, *GetNameSafe(this));
		return;
	}

	const FVector SpawnLoc = GetActorLocation();
	const FRotator SpawnRot = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	Aenemy_mobs* SpawnedEnemy = GetWorld()->SpawnActor<Aenemy_mobs>(EnemyClass, SpawnLoc, SpawnRot, SpawnParams);
	UE_LOG(LogTemp, Log, TEXT("[EnemySpawner] Spawned enemy. Spawner=%s Class=%s Result=%s"), *GetNameSafe(this), *GetNameSafe(EnemyClass.Get()), *GetNameSafe(SpawnedEnemy));
}
