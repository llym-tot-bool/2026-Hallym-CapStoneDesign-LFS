#include "Weapons/SLWeaponBase.h"

#include "Weapons/SLWeaponDataAsset.h"
#include "Weapons/SLWeaponTypes.h"
#include "Abilities/SLGE_WeaponDamage.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"

ASLWeaponBase::ASLWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SkeletalMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMeshComp->SetGenerateOverlapEvents(false);
	RootComponent = SkeletalMeshComp;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(RootComponent);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComp->SetGenerateOverlapEvents(false);

	DefaultDamageEffectClass = USLGE_WeaponDamage::StaticClass();
}

void ASLWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	// Apply the data asset's visuals if one is configured at spawn.
	if (WeaponData)
	{
		if (USkeletalMesh* SK = WeaponData->SkeletalMesh.LoadSynchronous())
		{
			SkeletalMeshComp->SetSkeletalMesh(SK);
		}
		if (UStaticMesh* SM = WeaponData->StaticMesh.LoadSynchronous())
		{
			StaticMeshComp->SetStaticMesh(SM);
		}
	}
}

USceneComponent* ASLWeaponBase::GetActiveMeshComponent() const
{
	if (SkeletalMeshComp && SkeletalMeshComp->GetSkeletalMeshAsset())
	{
		return SkeletalMeshComp;
	}
	return StaticMeshComp;
}

UAbilitySystemComponent* ASLWeaponBase::GetOwnerASC() const
{
	if (!OwnerCharacter) { return nullptr; }

	if (const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwnerCharacter))
	{
		return ASI->GetAbilitySystemComponent();
	}
	if (const APawn* Pawn = OwnerCharacter)
	{
		if (const IAbilitySystemInterface* PSAsi = Cast<IAbilitySystemInterface>(Pawn->GetPlayerState()))
		{
			return PSAsi->GetAbilitySystemComponent();
		}
	}
	return nullptr;
}

void ASLWeaponBase::Equip(ACharacter* NewOwnerCharacter)
{
	if (!NewOwnerCharacter || !WeaponData) { return; }

	OwnerCharacter = NewOwnerCharacter;
	SetOwner(NewOwnerCharacter);
	SetInstigator(NewOwnerCharacter);

	if (USkeletalMeshComponent* OwnerMesh = NewOwnerCharacter->GetMesh())
	{
		AttachToComponent(
			OwnerMesh,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			WeaponData->EquipSocket);
	}

	if (HasAuthority())
	{
		GrantAbilities();
		ApplyEquipEffects();
	}
}

void ASLWeaponBase::Unequip()
{
	StopHitDetection();

	if (HasAuthority())
	{
		ClearGrantedGAS();
	}

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	OwnerCharacter = nullptr;
	SetOwner(nullptr);
	SetInstigator(nullptr);
}

void ASLWeaponBase::Sheathe()
{
	if (!OwnerCharacter || !WeaponData) { return; }
	if (USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh())
	{
		AttachToComponent(
			OwnerMesh,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			WeaponData->SheatheSocket);
	}
}

void ASLWeaponBase::Draw()
{
	if (!OwnerCharacter || !WeaponData) { return; }
	if (USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh())
	{
		AttachToComponent(
			OwnerMesh,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			WeaponData->EquipSocket);
	}
}

void ASLWeaponBase::GrantAbilities()
{
	if (!WeaponData) { return; }
	UAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC) { return; }

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : WeaponData->GrantedAbilities)
	{
		if (!AbilityClass) { continue; }
		FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
		const FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
		GrantedAbilityHandles.Add(Handle);
	}
}

void ASLWeaponBase::ApplyEquipEffects()
{
	if (!WeaponData) { return; }
	UAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC) { return; }

	for (const TSubclassOf<UGameplayEffect>& EffectClass : WeaponData->EquipEffects)
	{
		if (!EffectClass) { continue; }
		FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
		Ctx.AddSourceObject(this);
		const FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(EffectClass, 1.f, Ctx);
		if (Spec.IsValid())
		{
			AppliedEquipEffectHandles.Add(ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get()));
		}
	}
}

void ASLWeaponBase::ClearGrantedGAS()
{
	UAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC) { return; }

	for (const FActiveGameplayEffectHandle& H : AppliedEquipEffectHandles)
	{
		ASC->RemoveActiveGameplayEffect(H);
	}
	AppliedEquipEffectHandles.Reset();

	for (const FGameplayAbilitySpecHandle& H : GrantedAbilityHandles)
	{
		ASC->ClearAbility(H);
	}
	GrantedAbilityHandles.Reset();
}

void ASLWeaponBase::StartHitDetection()
{
	if (bHitDetectionActive) { return; }
	bHitDetectionActive = true;
	HitActorsThisSwing.Reset();
	bHasPrevTrace = false;

	if (bAutoTickHitTrace)
	{
		SetActorTickEnabled(true);
	}
}

void ASLWeaponBase::StopHitDetection()
{
	if (!bHitDetectionActive) { return; }
	bHitDetectionActive = false;
	HitActorsThisSwing.Reset();
	bHasPrevTrace = false;
	SetActorTickEnabled(false);
}

void ASLWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bHitDetectionActive && bAutoTickHitTrace)
	{
		PerformHitTrace();
	}
}

void ASLWeaponBase::PerformHitTrace()
{
	if (!WeaponData) { return; }
	USceneComponent* MeshComp = GetActiveMeshComponent();
	if (!MeshComp) { return; }

	const FVector Start = MeshComp->DoesSocketExist(WeaponData->TraceStartSocket)
		? MeshComp->GetSocketLocation(WeaponData->TraceStartSocket)
		: GetActorLocation();

	const FVector End = MeshComp->DoesSocketExist(WeaponData->TraceEndSocket)
		? MeshComp->GetSocketLocation(WeaponData->TraceEndSocket)
		: Start + GetActorForwardVector() * 50.f;

	// Sweep from last frame's end to this frame's end so fast swings don't tunnel through targets.
	const FVector SweepFrom = bHasPrevTrace ? LastTraceEnd : Start;
	const FVector SweepTo = End;

	FCollisionShape Shape = FCollisionShape::MakeSphere(WeaponData->TraceRadius);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(SLWeaponSweep), false, this);
	Params.AddIgnoredActor(this);
	if (OwnerCharacter) { Params.AddIgnoredActor(OwnerCharacter); }

	TArray<FHitResult> Hits;
	GetWorld()->SweepMultiByChannel(
		Hits,
		SweepFrom,
		SweepTo,
		FQuat::Identity,
		HitTraceChannel,
		Shape,
		Params);

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActor == OwnerCharacter) { continue; }
		if (HitActorsThisSwing.Contains(HitActor)) { continue; }

		HitActorsThisSwing.Add(HitActor);
		OnWeaponHit.Broadcast(HitActor, Hit);

		// Auto-apply the damage GE on the server. Designers can disable this and
		// have the owning ability apply damage manually for special-case logic.
		if (bAutoApplyDamageOnHit && HasAuthority())
		{
			ApplyDamageToTarget(HitActor, Hit);
		}
	}

	LastTraceStart = Start;
	LastTraceEnd = End;
	bHasPrevTrace = true;
}

void ASLWeaponBase::ApplyDamageToTarget(AActor* Target, const FHitResult& Hit)
{
	if (!Target || !WeaponData) { return; }

	UAbilitySystemComponent* SourceASC = GetOwnerASC();
	if (!SourceASC) { return; }

	// Pull the target's ASC. Targets are expected to expose ASC either directly
	// (e.g. enemy pawns) or via PlayerState (player characters).
	UAbilitySystemComponent* TargetASC = nullptr;
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Target))
	{
		TargetASC = ASI->GetAbilitySystemComponent();
	}
	else if (APawn* Pawn = Cast<APawn>(Target))
	{
		if (IAbilitySystemInterface* PSAsi = Cast<IAbilitySystemInterface>(Pawn->GetPlayerState()))
		{
			TargetASC = PSAsi->GetAbilitySystemComponent();
		}
	}
	if (!TargetASC) { return; }

	// Per-weapon override wins; otherwise fall back to the project default.
	TSubclassOf<UGameplayEffect> EffectClass = WeaponData->DamageEffect
		? WeaponData->DamageEffect
		: DefaultDamageEffectClass;
	if (!EffectClass) { return; }

	FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
	Ctx.AddSourceObject(this);
	Ctx.AddInstigator(OwnerCharacter, this);
	Ctx.AddHitResult(Hit);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(EffectClass, /*Level*/ 1.f, Ctx);
	if (!SpecHandle.IsValid()) { return; }

	// Pack BaseDamage into the SetByCaller magnitude consumed by USLGE_WeaponDamage.
	const FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(SLCombatTags::SetByCaller_DamageBase, /*ErrorIfNotFound*/ false);
	if (DamageTag.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(DamageTag, WeaponData->BaseDamage);
	}

	// Optional poise damage rides along on the same spec for GEs that consume it.
	const FGameplayTag PoiseTag = FGameplayTag::RequestGameplayTag(SLCombatTags::SetByCaller_PoiseDamage, /*ErrorIfNotFound*/ false);
	if (PoiseTag.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(PoiseTag, WeaponData->PoiseDamage);
	}

	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}
