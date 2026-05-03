#include <EnemyMobsAIController.h>
#include "enemy_mobs.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"

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
	Aenemy_mobs* EnemyPawn = Cast<Aenemy_mobs>(GetPawn());
	auto ApplyMoveSpeed = [EnemyPawn](const float NewSpeed)
	{
		if (!EnemyPawn)
		{
			return;
		}

		if (UCharacterMovementComponent* MoveComp = EnemyPawn->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = NewSpeed;
		}
	};

	if (!EnemyPawn || !EnemyPawn->IsPlayerChaseEnabled())
	{
		StopMovement();
		CachedTargetActor = nullptr;
		if (EnemyPawn)
		{
			ApplyMoveSpeed(EnemyPawn->GetDefaultMoveSpeed());
		}
		return;
	}

	if (EnemyPawn->IsDebugDrawDetectionRangeEnabled())
	{
		const FVector PawnLocation = EnemyPawn->GetActorLocation();
		const FVector Forward = EnemyPawn->GetActorForwardVector().GetSafeNormal();
		const float ChaseDistance = EnemyPawn->GetMaxChaseDistance();
		const float ConeHalfAngleRad = FMath::DegreesToRadians(EnemyPawn->GetChaseDetectionHalfAngleDeg());
		const float DrawDuration = TargetSearchInterval + 0.05f;

		DrawDebugSphere(GetWorld(), PawnLocation, ChaseDistance, 36, FColor::Cyan, false, DrawDuration, 0, 1.2f);
		DrawDebugCone(GetWorld(), PawnLocation, Forward, ChaseDistance, ConeHalfAngleRad, ConeHalfAngleRad, 24, FColor::Green, false, DrawDuration, 0, 1.8f);
	}

	if (EnemyPawn->IsDebugDrawAttackRangeEnabled())
	{
		const FVector PawnLocation = EnemyPawn->GetActorLocation();
		const float AttackRange = EnemyPawn->GetBasicAttackRange();
		const float DrawDuration = TargetSearchInterval + 0.05f;
		const FVector AttackRangeCenter = PawnLocation + FVector(0.0f, 0.0f, 5.0f);

		DrawDebugSphere(GetWorld(), AttackRangeCenter, AttackRange, 32, FColor::Red, false, DrawDuration, 0, 1.5f);
	}

	AActor* TargetActor = ResolvePlayerTarget();
	const bool bHasLockedTarget = CachedTargetActor.IsValid();

	if (!IsValid(TargetActor))
	{
		StopMovement();
		CachedTargetActor = nullptr;
		ApplyMoveSpeed(EnemyPawn->GetDefaultMoveSpeed());
		return;
	}

	if (!bHasLockedTarget)
	{
		const float DistanceToTarget = FVector::Dist(EnemyPawn->GetActorLocation(), TargetActor->GetActorLocation());
		if (DistanceToTarget > EnemyPawn->GetMaxChaseDistance())
		{
			StopMovement();
			CachedTargetActor = nullptr;
			ApplyMoveSpeed(EnemyPawn->GetDefaultMoveSpeed());
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
			ApplyMoveSpeed(EnemyPawn->GetDefaultMoveSpeed());
			return;
		}
	}

	CachedTargetActor = TargetActor;
	ApplyMoveSpeed(EnemyPawn->GetChaseMoveSpeed());

	if (EnemyPawn->IsTargetInBasicAttackContact(TargetActor))
	{
		const bool bAttackStarted = EnemyPawn->TryBasicAttack(TargetActor);
		if (bAttackStarted || EnemyPawn->IsBasicAttackInProgress())
		{
			StopMovement();
			return;
		}
	}

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
