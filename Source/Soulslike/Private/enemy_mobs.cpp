#include "enemy_mobs.h"
#include "AbilitySystemComponent.h"
#include "SLCharacterAttributeSet.h"
#include "EnemyMobsAIController.h"
#include "Abilities/SLGE_WeaponDamage.h"
#include "Weapons/SLWeaponTypes.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffectTypes.h"

Aenemy_mobs::Aenemy_mobs()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<USLCharacterAttributeSet>(TEXT("AttributeSet"));

	AIControllerClass = AEnemyMobsAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	bUseControllerRotationYaw = false;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = DefaultMoveSpeed;
	}
}

UAbilitySystemComponent* Aenemy_mobs::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void Aenemy_mobs::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Tags.AddUnique(FName("Enemy"));

	if (AttributeSet && AttributeSet->GetPower() <= 0.0f)
	{
		AttributeSet->SetMaxPower(FMath::Max(AttributeSet->GetMaxPower(), InitialPowerStat));
		AttributeSet->SetPower(InitialPowerStat);
	}

	if (bEnablePeriodicMove)
	{
		StartPeriodicMove();
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("[EnemyMob] BeginPlay: Name=%s HasAuthority=%d MontageA=%s MontageB=%s AnimInstance=%s AttackRange=%.1f Cooldown=%.2f"),
		*GetNameSafe(this),
		HasAuthority() ? 1 : 0,
		*GetNameSafe(BasicAttackMontage),
		*GetNameSafe(BasicAttackMontageAlt),
		*GetNameSafe(GetMesh() ? GetMesh()->GetAnimInstance() : nullptr),
		BasicAttackRange,
		BasicAttackCooldown);
}

void Aenemy_mobs::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BasicAttackHitTimer);
	}
	StopPeriodicMove();
	Super::EndPlay(EndPlayReason);
}

void Aenemy_mobs::StartPeriodicMove()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			PeriodicMoveTimer,
			this,
			&Aenemy_mobs::MoveToRandomReachableLocation,
			PeriodicMoveInterval,
			true);
	}
}

void Aenemy_mobs::StopPeriodicMove()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PeriodicMoveTimer);
	}
}

void Aenemy_mobs::MoveToRandomReachableLocation()
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

float Aenemy_mobs::ComputeBasicAttackDamage() const
{
	return 15.f;
}

bool Aenemy_mobs::TryBasicAttack(AActor* TargetActor)
{
	if (!HasAuthority() || !IsValid(TargetActor) || !AbilitySystemComponent || bBasicAttackInProgress)
	{
		return false;
	}

	UAbilitySystemComponent* TargetASC = nullptr;
	if (IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(TargetActor))
	{
		TargetASC = TargetASI->GetAbilitySystemComponent();
	}
	if (!TargetASC)
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

	UAnimMontage* MontageToPlay = nullptr;
	if (BasicAttackMontage && BasicAttackMontageAlt)
	{
		MontageToPlay = FMath::RandBool() ? BasicAttackMontage.Get() : BasicAttackMontageAlt.Get();
	}
	else if (BasicAttackMontage)
	{
		MontageToPlay = BasicAttackMontage;
	}
	else if (BasicAttackMontageAlt)
	{
		MontageToPlay = BasicAttackMontageAlt;
	}

	if (!MontageToPlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemyMob] Attack montage is not assigned: %s"), *GetNameSafe(this));
		return false;
	}

	MulticastPlayBasicAttackMontage(MontageToPlay, 1.0f);

	PendingAttackTarget = TargetActor;
	bBasicAttackInProgress = true;
	LastBasicAttackTime = Now;

	float ResolveDelay = MontageToPlay->GetPlayLength();
	if (ResolveDelay <= 0.0f)
	{
		ResolveDelay = BasicAttackHitDelay;
	}

	if (ResolveDelay <= 0.0f)
	{
		ResolveBasicAttackHit();
		return true;
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("[EnemyMob] Attack started: Attacker=%s Target=%s Montage=%s ResolveDelay=%.2f"),
		*GetNameSafe(this),
		*GetNameSafe(TargetActor),
		*GetNameSafe(MontageToPlay),
		ResolveDelay);

	World->GetTimerManager().SetTimer(
		BasicAttackHitTimer,
		this,
		&Aenemy_mobs::ResolveBasicAttackHit,
		ResolveDelay,
		false);

	return true;
}

bool Aenemy_mobs::IsTargetInBasicAttackContact(AActor* TargetActor) const
{
	return IsTargetTouchingAttackRange(TargetActor);
}

bool Aenemy_mobs::IsTargetTouchingAttackRange(AActor* TargetActor) const
{
	if (!IsValid(TargetActor))
	{
		return false;
	}

	const float Dist2D = FVector::Dist2D(GetActorLocation(), TargetActor->GetActorLocation());
	return Dist2D <= BasicAttackRange;
}

void Aenemy_mobs::ResolveBasicAttackHit()
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
		UE_LOG(LogTemp, Log, TEXT("[EnemyMob] Attack missed (out of attack range): Attacker=%s Target=%s"), *GetNameSafe(this), *GetNameSafe(TargetActor));
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

	UE_LOG(LogTemp, Log, TEXT("[EnemyMob] Attack success: Attacker=%s Target=%s Damage=%.2f"), *GetNameSafe(this), *GetNameSafe(TargetActor), FinalDamage);
}

void Aenemy_mobs::MulticastPlayBasicAttackMontage_Implementation(UAnimMontage* MontageToPlay, float PlayRate)
{
	if (!MontageToPlay)
	{
		return;
	}

	const float Played = PlayAnimMontage(MontageToPlay, PlayRate);
	if (Played <= 0.0f)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[EnemyMob] Failed to play attack montage: Mob=%s Montage=%s AnimInstance=%s"),
			*GetNameSafe(this),
			*GetNameSafe(MontageToPlay),
			*GetNameSafe(GetMesh() ? GetMesh()->GetAnimInstance() : nullptr));
	}
}
