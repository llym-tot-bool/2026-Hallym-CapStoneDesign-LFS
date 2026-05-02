#include <EnemyMobsAIController.h>
#include "enemy_mobs.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AEnemyMobsAIController::AEnemyMobsAIController()
{
	bAttachToPawn = true;
	bStartAILogicOnPossess = true;
}

void AEnemyMobsAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UpdateChaseTarget();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TargetSearchTimer,
			this,
			&AEnemyMobsAIController::UpdateChaseTarget,
			TargetSearchInterval,
			true);
	}
}

void AEnemyMobsAIController::OnUnPossess()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TargetSearchTimer);
	}

	StopMovement();
	CachedTargetActor = nullptr;

	Super::OnUnPossess();
}

void AEnemyMobsAIController::UpdateChaseTarget()
{
	const Aenemy_mobs* EnemyPawn = Cast<Aenemy_mobs>(GetPawn());
	if (!EnemyPawn || !EnemyPawn->IsPlayerChaseEnabled())
	{
		StopMovement();
		CachedTargetActor = nullptr;
		return;
	}

	AActor* TargetActor = ResolvePlayerTarget();

	if (!IsValid(TargetActor))
	{
		StopMovement();
		CachedTargetActor = nullptr;
		return;
	}

	const float DistanceToTarget = FVector::Dist(EnemyPawn->GetActorLocation(), TargetActor->GetActorLocation());
	if (DistanceToTarget > EnemyPawn->GetMaxChaseDistance())
	{
		StopMovement();
		CachedTargetActor = nullptr;
		return;
	}

	const FVector ToTarget2D = (TargetActor->GetActorLocation() - EnemyPawn->GetActorLocation()).GetSafeNormal2D();
	const FVector Forward2D = EnemyPawn->GetActorForwardVector().GetSafeNormal2D();
	const float ConeHalfAngleDeg = EnemyPawn->GetChaseDetectionHalfAngleDeg();
	const float CosThreshold = FMath::Cos(FMath::DegreesToRadians(ConeHalfAngleDeg));
	const float DotToTarget = FVector::DotProduct(Forward2D, ToTarget2D);

	if (DotToTarget < CosThreshold)
	{
		StopMovement();
		CachedTargetActor = nullptr;
		return;
	}

	CachedTargetActor = TargetActor;

	float AcceptanceRadius = 120.0f;
	AcceptanceRadius = EnemyPawn->GetChaseAcceptanceRadius();

	MoveToActor(TargetActor, AcceptanceRadius, true, true, true, nullptr, true);
}

AActor* AEnemyMobsAIController::ResolvePlayerTarget() const
{
	if (!GetWorld())
	{
		return nullptr;
	}

	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		return PlayerPawn;
	}

	return nullptr;
}
