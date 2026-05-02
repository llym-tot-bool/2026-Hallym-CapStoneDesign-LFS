// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "SLCharacterAttributeSet.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

ABoss::ABoss()
{
	PrimaryActorTick.bCanEverTick = false;
	bUseControllerRotationYaw = false;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<USLCharacterAttributeSet>(TEXT("AttributeSet"));

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = 280.0f;
		MoveComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
		MoveComp->bOrientRotationToMovement = true;
	}
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Tags.AddUnique(FName("Boss"));
	Tags.AddUnique(FName("Enemy"));

	StartChase();

	if (bEnablePeriodicMove)
	{
		StartPeriodicMove();
	}

	if (IntroMontage)
	{
		PlayBossMontage(IntroMontage);
	}
}

void ABoss::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopChase();
	StopPeriodicMove();
	Super::EndPlay(EndPlayReason);
}

UAbilitySystemComponent* ABoss::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABoss::StartChase()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			ChaseTimer,
			this,
			&ABoss::UpdateChaseTarget,
			TargetSearchInterval,
			true);
	}
}

void ABoss::StopChase()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ChaseTimer);
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}
}

void ABoss::UpdateChaseTarget()
{
	if (!bEnablePlayerChase)
	{
		return;
	}

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		return;
	}

	APawn* TargetPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!IsValid(TargetPawn))
	{
		return;
	}

	const float DistanceToTarget = FVector::Dist(GetActorLocation(), TargetPawn->GetActorLocation());
	if (DistanceToTarget > MaxChaseDistance)
	{
		AIController->StopMovement();
		return;
	}

	AIController->MoveToActor(TargetPawn, ChaseAcceptanceRadius, true, true, true, nullptr, true);
}

void ABoss::StartPeriodicMove()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			PeriodicMoveTimer,
			this,
			&ABoss::MoveToRandomReachableLocation,
			PeriodicMoveInterval,
			true);
	}
}

void ABoss::StopPeriodicMove()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PeriodicMoveTimer);
	}
}

void ABoss::MoveToRandomReachableLocation()
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		return;
	}

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSystem)
	{
		return;
	}

	FNavLocation RandomLocation;
	const bool bFound = NavSystem->GetRandomReachablePointInRadius(GetActorLocation(), PeriodicMoveRadius, RandomLocation);
	if (!bFound)
	{
		return;
	}

	AIController->MoveToLocation(RandomLocation.Location, ChaseAcceptanceRadius, true, true, true, false, nullptr, true);
}

bool ABoss::PlayBossMontage(UAnimMontage* MontageToPlay, float PlayRate)
{
	if (!MontageToPlay)
	{
		return false;
	}

	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		return false;
	}

	return AnimInstance->Montage_Play(MontageToPlay, PlayRate) > 0.0f;
}

bool ABoss::PlayDefaultAttackMontage(float PlayRate)
{
	return PlayBossMontage(AttackMontage, PlayRate);
}

bool ABoss::PlayDeathMontage(float PlayRate)
{
	return PlayBossMontage(DeathMontage, PlayRate);
}

float ABoss::GetGroundSpeed() const
{
	FVector Velocity2D = GetVelocity();
	Velocity2D.Z = 0.0f;
	return Velocity2D.Size();
}

bool ABoss::IsMoving() const
{
	return GetGroundSpeed() > 3.0f;
}

bool ABoss::IsDead() const
{
	if (!AttributeSet)
	{
		return false;
	}

	return AttributeSet->GetHealth() <= 0.0f;
}

