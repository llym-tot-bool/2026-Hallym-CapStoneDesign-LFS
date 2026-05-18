#include "enemy_mobs.h"
#include "AbilitySystemComponent.h"
#include "SLCharacterAttributeSet.h"
#include "EnemyMobsAIController.h"
#include "Abilities/SLGE_WeaponDamage.h"
#include "Weapons/SLWeaponTypes.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"

namespace
{
	UAbilitySystemComponent* ResolveEnemyTargetASC(AActor* TargetActor)
	{
		if (!IsValid(TargetActor))
		{
			return nullptr;
		}

		if (IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(TargetActor))
		{
			return TargetASI->GetAbilitySystemComponent();
		}

		if (APawn* TargetPawn = Cast<APawn>(TargetActor))
		{
			if (IAbilitySystemInterface* PlayerStateASI = Cast<IAbilitySystemInterface>(TargetPawn->GetPlayerState()))
			{
				return PlayerStateASI->GetAbilitySystemComponent();
			}
		}

		return nullptr;
	}
}

Aenemy_mobs::Aenemy_mobs()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

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

	if (AbilitySystemComponent)
	{
		const FGameplayTag GroggyTag = FGameplayTag::RequestGameplayTag(SLCombatTags::State_Groggy, false);
		if (GroggyTag.IsValid())
		{
			AbilitySystemComponent->RegisterGameplayTagEvent(GroggyTag, EGameplayTagEventType::NewOrRemoved)
				.AddUObject(this, &Aenemy_mobs::OnGroggyTagChanged);
			bIsGroggy = AbilitySystemComponent->HasMatchingGameplayTag(GroggyTag);
		}
	}

	if (AttributeSet && AttributeSet->GetPower() <= 0.0f)
	{
		AttributeSet->SetMaxPower(FMath::Max(AttributeSet->GetMaxPower(), InitialPowerStat));
		AttributeSet->SetPower(InitialPowerStat);
	}

	if (AttributeSet && AttributeSet->GetMaxGroggy() <= 1.0f)
	{
		AttributeSet->SetMaxGroggy(InitialGroggyStat);
	}
	if (AttributeSet && AttributeSet->GetGroggy() <= 0.0f)
	{
		AttributeSet->SetGroggy(AttributeSet->GetMaxGroggy());
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
	const float Power = AttributeSet ? AttributeSet->GetPower() : 0.0f;
	return BasicAttackBaseDamage + (Power * BasicAttackPowerScale);
}

bool Aenemy_mobs::TryBasicAttack(AActor* TargetActor)
{
	if (!HasAuthority() || !IsValid(TargetActor) || !AbilitySystemComponent || bBasicAttackInProgress || bIsGroggy)
	{
		if (HasAuthority() && bBasicAttackInProgress)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[EnemyMob] TryBasicAttack blocked: already in progress (%s)"), *GetNameSafe(this));
		}
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
		UE_LOG(LogTemp, Verbose, TEXT("[EnemyMob] TryBasicAttack blocked by cooldown. Attacker=%s"), *GetNameSafe(this));
		return false;
	}

	if (!IsTargetTouchingAttackRange(TargetActor))
	{
		UE_LOG(LogTemp, Verbose, TEXT("[EnemyMob] TryBasicAttack blocked: target out of range. Attacker=%s Target=%s"), *GetNameSafe(this), *GetNameSafe(TargetActor));
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

	UE_LOG(
		LogTemp,
		Log,
		TEXT("[EnemyMob] Attack started: Attacker=%s Target=%s Montage=%s (Damage by AnimNotify)"),
		*GetNameSafe(this),
		*GetNameSafe(TargetActor),
		*GetNameSafe(MontageToPlay));

	// Safety: if the montage notify is missing (or montage failed to play on mesh),
	// release attack state so AI doesn't get stuck forever.
	float NotifyTimeout = MontageToPlay->GetPlayLength() + 0.2f;
	if (NotifyTimeout <= 0.2f)
	{
		NotifyTimeout = 1.0f;
	}
	World->GetTimerManager().SetTimer(BasicAttackHitTimer, this, &Aenemy_mobs::OnBasicAttackNotifyTimeout, NotifyTimeout, false);

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
	if (!bBasicAttackInProgress)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BasicAttackHitTimer);
	}

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

	UAbilitySystemComponent* TargetASC = ResolveEnemyTargetASC(TargetActor);
	const float FinalDamage = ComputeBasicAttackDamage();

	if (!TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemyMob] Attack hit but target ASC was not found. GAS damage skipped. Attacker=%s Target=%s"), *GetNameSafe(this), *GetNameSafe(TargetActor));
		return;
	}

	FGameplayEffectContextHandle Ctx = AbilitySystemComponent->MakeEffectContext();
	Ctx.AddInstigator(this, this);
	Ctx.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(USLGE_WeaponDamage::StaticClass(), 1.0f, Ctx);
	if (SpecHandle.IsValid())
	{
		const FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(SLCombatTags::SetByCaller_DamageBase, false);
		if (DamageTag.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(DamageTag, FinalDamage);
			AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[EnemyMob] Attack success: Attacker=%s Target=%s Damage=%.2f"), *GetNameSafe(this), *GetNameSafe(TargetActor), FinalDamage);
}

void Aenemy_mobs::OnBasicAttackDamageNotify()
{
	if (!HasAuthority())
	{
		return;
	}

	ResolveBasicAttackHit();
}

void Aenemy_mobs::OnBasicAttackSpeedResetNotify()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		return;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	if (UAnimMontage* ActiveMontage = AnimInstance->GetCurrentActiveMontage())
	{
		const float BaseRateScale = ActiveMontage->RateScale;
		const float PlayRateForOneX = FMath::IsNearlyZero(BaseRateScale) ? 1.0f : (1.0f / BaseRateScale);
		AnimInstance->Montage_SetPlayRate(ActiveMontage, PlayRateForOneX);
	}
}

void Aenemy_mobs::OnBasicAttackNotifyTimeout()
{
	if (!bBasicAttackInProgress)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[EnemyMob] Attack notify timeout: montage notify missing or montage did not trigger notify. Attacker=%s"), *GetNameSafe(this));
	bBasicAttackInProgress = false;
	PendingAttackTarget = nullptr;
}

void Aenemy_mobs::OnGroggyTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bIsGroggy = NewCount > 0;
	if (!bIsGroggy)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BasicAttackHitTimer);
	}
	bBasicAttackInProgress = false;
	PendingAttackTarget = nullptr;

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	// Knock the enemy backward a fixed distance when groggy is triggered.
	if (GroggyKnockbackDistance > 0.0f)
	{
		const float Duration = FMath::Max(0.05f, GroggyKnockbackDuration);
		const float KnockbackSpeed = GroggyKnockbackDistance / Duration;
		const FVector BackwardDir = -GetActorForwardVector().GetSafeNormal2D();
		LaunchCharacter(BackwardDir * KnockbackSpeed, true, false);
	}

	if (GroggyMontage)
	{
		MulticastPlayGroggyMontage(GroggyMontage, 1.0f);
	}

	UE_LOG(LogTemp, Log, TEXT("[EnemyMob] Entered groggy state: %s"), *GetNameSafe(this));
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

void Aenemy_mobs::MulticastPlayGroggyMontage_Implementation(UAnimMontage* MontageToPlay, float PlayRate)
{
	if (!MontageToPlay)
	{
		return;
	}

	const float Played = PlayAnimMontage(MontageToPlay, PlayRate);
	if (Played <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemyMob] Failed to play groggy montage: Mob=%s Montage=%s"), *GetNameSafe(this), *GetNameSafe(MontageToPlay));
	}
}
