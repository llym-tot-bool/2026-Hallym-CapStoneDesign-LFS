#include <EnemyMobsAIController.h>
#include "enemy_mobs.h"
#include "Boss.h"
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
	ABoss* BossPawn = Cast<ABoss>(GetPawn());
	ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn());

	auto ApplyMoveSpeed = [ControlledCharacter](const float NewSpeed)
	{
		if (!ControlledCharacter)
		{
			return;
		}

		if (UCharacterMovementComponent* MoveComp = ControlledCharacter->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = NewSpeed;
		}
	};

	const bool bCanChasePlayer = EnemyPawn ? EnemyPawn->IsPlayerChaseEnabled() : (BossPawn ? BossPawn->IsPlayerChaseEnabled() : false);
	if ((!EnemyPawn && !BossPawn) || !bCanChasePlayer)
	{
		StopMovement();
		CachedTargetActor = nullptr;
		const float DefaultSpeed = EnemyPawn ? EnemyPawn->GetDefaultMoveSpeed() : (BossPawn ? BossPawn->GetDefaultMoveSpeed() : 0.0f);
		ApplyMoveSpeed(DefaultSpeed);
		return;
	}

	if ((EnemyPawn && EnemyPawn->IsGroggy()) || (BossPawn && BossPawn->IsGroggy()))
	{
		StopMovement();
		if (ControlledCharacter)
		{
			if (UCharacterMovementComponent* MoveComp = ControlledCharacter->GetCharacterMovement())
			{
				MoveComp->StopMovementImmediately();
			}
		}
		ApplyMoveSpeed(EnemyPawn ? EnemyPawn->GetDefaultMoveSpeed() : BossPawn->GetDefaultMoveSpeed());
		CachedTargetActor = nullptr;
		return;
	}

	const bool bAttackMontagePlaying = EnemyPawn ? EnemyPawn->IsBasicAttackMontagePlaying() : BossPawn->IsBasicAttackMontagePlaying();
	if (bAttackMontagePlaying)
	{
		StopMovement();
		return;
	}

	const bool bDebugDrawDetection = EnemyPawn ? EnemyPawn->IsDebugDrawDetectionRangeEnabled() : BossPawn->IsDebugDrawDetectionRangeEnabled();
	if (bDebugDrawDetection)
	{
		const FVector PawnLocation = GetPawn()->GetActorLocation();
		const FVector Forward = GetPawn()->GetActorForwardVector().GetSafeNormal();
		const float ChaseDistance = EnemyPawn ? EnemyPawn->GetMaxChaseDistance() : BossPawn->GetMaxChaseDistance();
		const float ConeHalfAngleRad = FMath::DegreesToRadians(EnemyPawn ? EnemyPawn->GetChaseDetectionHalfAngleDeg() : BossPawn->GetChaseDetectionHalfAngleDeg());
		const float DrawDuration = TargetSearchInterval + 0.05f;

		DrawDebugSphere(GetWorld(), PawnLocation, ChaseDistance, 36, FColor::Cyan, false, DrawDuration, 0, 1.2f);
		DrawDebugCone(GetWorld(), PawnLocation, Forward, ChaseDistance, ConeHalfAngleRad, ConeHalfAngleRad, 24, FColor::Green, false, DrawDuration, 0, 1.8f);
	}

	const bool bDebugDrawAttackRange = EnemyPawn ? EnemyPawn->IsDebugDrawAttackRangeEnabled() : BossPawn->IsDebugDrawAttackRangeEnabled();
	if (bDebugDrawAttackRange)
	{
		const FVector PawnLocation = GetPawn()->GetActorLocation();
		const float AttackRange = EnemyPawn ? EnemyPawn->GetBasicAttackRange() : BossPawn->GetBasicAttackRange();
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
		ApplyMoveSpeed(EnemyPawn ? EnemyPawn->GetDefaultMoveSpeed() : BossPawn->GetDefaultMoveSpeed());
		return;
	}

	if (!bHasLockedTarget)
	{
		const FVector PawnLocation = GetPawn()->GetActorLocation();
		const float MaxChaseDistance = EnemyPawn ? EnemyPawn->GetMaxChaseDistance() : BossPawn->GetMaxChaseDistance();
		const float DistanceToTarget = FVector::Dist(PawnLocation, TargetActor->GetActorLocation());
		if (DistanceToTarget > MaxChaseDistance)
		{
			StopMovement();
			CachedTargetActor = nullptr;
			ApplyMoveSpeed(EnemyPawn ? EnemyPawn->GetDefaultMoveSpeed() : BossPawn->GetDefaultMoveSpeed());
			return;
		}

		// Once damaged, both enemy and boss can force-aggro without front-cone restriction.
		const bool bDamageAggro = EnemyPawn ? EnemyPawn->HasTakenDamageAggro() : (BossPawn ? BossPawn->HasTakenDamageAggro() : false);

		// Enemy front-cone gate applies only before damage aggro.
		if (EnemyPawn && !bDamageAggro)
		{
			const FVector ToTarget2D = (TargetActor->GetActorLocation() - PawnLocation).GetSafeNormal2D();
			const FVector Forward2D = GetPawn()->GetActorForwardVector().GetSafeNormal2D();
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
	}

	CachedTargetActor = TargetActor;
	ApplyMoveSpeed(EnemyPawn ? EnemyPawn->GetChaseMoveSpeed() : BossPawn->GetChaseMoveSpeed());

	const bool bInAttackRange = EnemyPawn ? EnemyPawn->IsTargetInBasicAttackContact(TargetActor) : BossPawn->IsTargetInBasicAttackContact(TargetActor);
	if (bInAttackRange)
	{
		const bool bAttackStarted = EnemyPawn ? EnemyPawn->TryBasicAttack(TargetActor) : BossPawn->TryBasicAttack(TargetActor);
		const bool bAttackInProgress = EnemyPawn ? EnemyPawn->IsBasicAttackInProgress() : BossPawn->IsBasicAttackInProgress();
		UE_LOG(
			LogTemp,
			Log,
			TEXT("[EnemyAI] AttackCheck Pawn=%s Target=%s InRange=1 Started=%d InProgress=%d Dist2D=%.1f"),
			*GetNameSafe(GetPawn()),
			*GetNameSafe(TargetActor),
			bAttackStarted ? 1 : 0,
			bAttackInProgress ? 1 : 0,
			FVector::Dist2D(GetPawn()->GetActorLocation(), TargetActor->GetActorLocation()));
		if (bAttackStarted || bAttackInProgress)
		{
			StopMovement();
			return;
		}
	}
	else
	{
		UE_LOG(
			LogTemp,
			Verbose,
			TEXT("[EnemyAI] OutOfRange Pawn=%s Target=%s Dist2D=%.1f"),
			*GetNameSafe(GetPawn()),
			*GetNameSafe(TargetActor),
			FVector::Dist2D(GetPawn()->GetActorLocation(), TargetActor->GetActorLocation()));
	}

	const float AcceptanceRadius = EnemyPawn ? EnemyPawn->GetChaseAcceptanceRadius() : BossPawn->GetChaseAcceptanceRadius();

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
