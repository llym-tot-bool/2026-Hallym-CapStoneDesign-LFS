// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"
#include "AbilitySystemComponent.h"
#include "EnemyMobsAIController.h"
#include "Abilities/SLGE_WeaponDamage.h"
#include "Weapons/SLWeaponTypes.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "SLCharacterAttributeSet.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayEffectTypes.h"

ABoss::ABoss()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AEnemyMobsAIController::StaticClass();

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<USLCharacterAttributeSet>(TEXT("AttributeSet"));

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = DefaultMoveSpeed;
		MoveComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
		MoveComp->bOrientRotationToMovement = true;
		MoveComp->bUseControllerDesiredRotation = false;
	}
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Tags.AddUnique(FName("Boss"));
	Tags.AddUnique(FName("Enemy"));

	if (bEnablePeriodicMove)
	{
		StartPeriodicMove();
	}

	if (IntroMontage)
	{
		PlayBossMontage(IntroMontage);
	}
}

void ABoss::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bFaceMovementDirection || IsDead())
	{
		return;
	}

	const FVector Velocity2D = FVector(GetVelocity().X, GetVelocity().Y, 0.0f);
	if (Velocity2D.SizeSquared() <= FMath::Square(3.0f))
	{
		return;
	}

	const FRotator CurrentRot = GetActorRotation();
	const FRotator DesiredRot = Velocity2D.Rotation();
	const FRotator NewRot = FMath::RInterpTo(CurrentRot, FRotator(0.0f, DesiredRot.Yaw, 0.0f), DeltaSeconds, MoveFacingInterpSpeed);
	SetActorRotation(NewRot);
}

void ABoss::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BasicAttackHitTimer);
	}

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
	bEnablePlayerChase = true;
}

void ABoss::StopChase()
{
	bEnablePlayerChase = false;

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

float ABoss::ComputeBasicAttackDamage() const
{
	const float Power = AttributeSet ? AttributeSet->GetPower() : 0.0f;
	return BasicAttackBaseDamage + (Power * BasicAttackPowerScale);
}

bool ABoss::TryBasicAttack(AActor* TargetActor)
{
	if (!HasAuthority() || !IsValid(TargetActor) || bBasicAttackInProgress || !AttackMontage)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const float Now = World->GetTimeSeconds();
	if ((Now - LastBasicAttackTime) < BasicAttackCooldown)
	{
		return false;
	}

	if (!IsTargetTouchingAttackRange(TargetActor))
	{
		return false;
	}

	if (!PlayDefaultAttackMontage(1.0f))
	{
		return false;
	}

	PendingAttackTarget = TargetActor;
	bBasicAttackInProgress = true;
	LastBasicAttackTime = Now;

	float ResolveDelay = AttackMontage->GetPlayLength();
	if (ResolveDelay <= 0.0f)
	{
		ResolveDelay = BasicAttackHitDelay;
	}

	if (ResolveDelay <= 0.0f)
	{
		ResolveBasicAttackHit();
		return true;
	}

	World->GetTimerManager().SetTimer(
		BasicAttackHitTimer,
		this,
		&ABoss::ResolveBasicAttackHit,
		ResolveDelay,
		false);

	return true;
}

bool ABoss::IsTargetInBasicAttackContact(AActor* TargetActor) const
{
	return IsTargetTouchingAttackRange(TargetActor);
}

bool ABoss::IsTargetTouchingAttackRange(AActor* TargetActor) const
{
	if (!IsValid(TargetActor))
	{
		return false;
	}

	const float Dist2D = FVector::Dist2D(GetActorLocation(), TargetActor->GetActorLocation());
	return Dist2D <= BasicAttackRange;
}

void ABoss::ResolveBasicAttackHit()
{
	bBasicAttackInProgress = false;

	AActor* TargetActor = PendingAttackTarget.Get();
	PendingAttackTarget = nullptr;

	if (!IsValid(TargetActor) || !AbilitySystemComponent)
	{
		return;
	}

	if (!IsTargetTouchingAttackRange(TargetActor))
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = nullptr;
	if (IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(TargetActor))
	{
		TargetASC = TargetASI->GetAbilitySystemComponent();
	}
	if (!TargetASC)
	{
		return;
	}

	FGameplayEffectContextHandle Ctx = AbilitySystemComponent->MakeEffectContext();
	Ctx.AddInstigator(this, this);
	Ctx.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(USLGE_WeaponDamage::StaticClass(), 1.0f, Ctx);
	if (!SpecHandle.IsValid())
	{
		return;
	}

	const FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(SLCombatTags::SetByCaller_DamageBase, false);
	if (!DamageTag.IsValid())
	{
		return;
	}

	const float FinalDamage = ComputeBasicAttackDamage();
	SpecHandle.Data->SetSetByCallerMagnitude(DamageTag, FinalDamage);
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}

