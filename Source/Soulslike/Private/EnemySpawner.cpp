#include "EnemySpawner.h"
#include "Engine/World.h"

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
		return;
	}

	const FVector SpawnLoc = GetActorLocation();
	const FRotator SpawnRot = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(EnemyClass, SpawnLoc, SpawnRot, SpawnParams);
}
