#include "BossSpawner.h"

#include "Boss.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "SoulslikeGameMode.h"

ABossSpawner::ABossSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void ABossSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void ABossSpawner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bShowDetectionDebug && GetWorld())
	{
		const FVector Origin = GetActorLocation();
		const FVector Forward = GetActorForwardVector();
		const FVector Direction = Forward.GetSafeNormal();
		const float HalfAngleRad = FMath::DegreesToRadians(DetectionHalfAngleDeg);
		const float ConeWidth = DetectionRange * FMath::Tan(HalfAngleRad);
		DrawDebugCone(
			GetWorld(),
			Origin,
			Direction,
			DetectionRange,
			HalfAngleRad,
			HalfAngleRad,
			24,
			FColor::Yellow,
			false,
			0.0f,
			0,
			1.5f);
		DrawDebugCircle(GetWorld(), Origin + Direction * DetectionRange, ConeWidth, 32, FColor::Yellow, false, 0.0f, 0, 1.0f, FVector::UpVector, FVector::RightVector, false);
	}

	if (bSpawnOnlyOnce && bHasSpawnedBoss)
	{
		return;
	}

	TryDetectAndSpawn();
}

bool ABossSpawner::IsPawnDetectedInFront(const APawn* Pawn) const
{
	if (!Pawn)
	{
		return false;
	}

	const FVector ToTarget = Pawn->GetActorLocation() - GetActorLocation();
	const float DistSq = ToTarget.SizeSquared();
	if (DistSq > FMath::Square(DetectionRange))
	{
		return false;
	}

	const FVector ToTargetDir = ToTarget.GetSafeNormal();
	const float Dot = FVector::DotProduct(GetActorForwardVector().GetSafeNormal(), ToTargetDir);
	const float MinDot = FMath::Cos(FMath::DegreesToRadians(DetectionHalfAngleDeg));
	return Dot >= MinDot;
}

void ABossSpawner::TryDetectAndSpawn()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn || !PlayerPawn->IsPlayerControlled())
	{
		return;
	}

	if (!IsPawnDetectedInFront(PlayerPawn))
	{
		return;
	}

	ABoss* SpawnedBoss = SpawnBoss();
	if (!SpawnedBoss)
	{
		return;
	}

	if (bDestroyAfterSpawn)
	{
		Destroy();
	}
}

ABoss* ABossSpawner::SpawnBoss()
{
	if ((bSpawnOnlyOnce && bHasSpawnedBoss) || !BossClass || !GetWorld())
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ABoss* SpawnedBoss = GetWorld()->SpawnActor<ABoss>(BossClass, GetActorLocation(), GetActorRotation(), SpawnParams);

	if (SpawnedBoss)
	{
		bHasSpawnedBoss = true;

		if (ASoulslikeGameMode* GameMode = GetWorld()->GetAuthGameMode<ASoulslikeGameMode>())
		{
			GameMode->RegisterSpawnedBoss(SpawnedBoss);
		}

		UE_LOG(LogTemp, Log, TEXT("[BossSpawner] Spawned boss. Spawner=%s Boss=%s"), *GetNameSafe(this), *GetNameSafe(SpawnedBoss));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossSpawner] Failed to spawn boss. Spawner=%s BossClass=%s"), *GetNameSafe(this), *GetNameSafe(BossClass.Get()));
	}

	return SpawnedBoss;
}
