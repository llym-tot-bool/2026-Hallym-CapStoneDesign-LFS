#include "EnemySpawner.h"
#include "Engine/World.h"
#include "enemy_mobs.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemySpawner::TrySpawnEnemyOnce(int level)
{
	if (!EnemyClass || !GetWorld())
	{
		return;
	}

	const float EffectiveSpawnChance = FMath::Clamp(SpawnChance + SpawnChanceIncreasePerMapLoad * level, 0.0f, MaxSpawnChance);
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
