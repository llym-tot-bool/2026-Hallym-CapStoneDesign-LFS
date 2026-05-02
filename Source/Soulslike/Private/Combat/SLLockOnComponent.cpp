#include "Combat/SLLockOnComponent.h"

#include "Weapons/SLWeaponTypes.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"

USLLockOnComponent::USLLockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USLLockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	// Default to Pawn-only acquisition so designers don't have to remember to
	// fill this in on every BP. They can still override in defaults.
	if (TargetObjectTypes.Num() == 0)
	{
		TargetObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	}
}

void USLLockOnComponent::ToggleLockOn()
{
	if (IsLocked())
	{
		ClearLock();
		return;
	}

	if (AActor* Best = FindBestTargetInCone())
	{
		SetTarget(Best);
	}
}

void USLLockOnComponent::ClearLock()
{
	if (CurrentTarget)
	{
		SetTarget(nullptr);
	}
}

void USLLockOnComponent::SwitchTarget(float Direction)
{
	if (!IsLocked() || FMath::IsNearlyZero(Direction)) { return; }

	APawn* Owner = Cast<APawn>(GetOwner());
	if (!Owner || !Owner->GetController()) { return; }

	const FVector OwnerLoc = Owner->GetActorLocation();
	const FVector ToCurrent = (CurrentTarget->GetActorLocation() - OwnerLoc).GetSafeNormal2D();

	TArray<AActor*> Overlapped;
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		OwnerLoc,
		BreakRange,
		TargetObjectTypes,
		nullptr,
		{ Owner, CurrentTarget },
		Overlapped);

	AActor* Best = nullptr;
	float BestScore = TNumericLimits<float>::Max();

	for (AActor* Candidate : Overlapped)
	{
		if (!IsValidCandidate(Candidate)) { continue; }

		const FVector ToCandidate = (Candidate->GetActorLocation() - OwnerLoc).GetSafeNormal2D();
		// Cross-Z sign tells us which side the candidate is on (right vs left).
		const float Side = FVector::CrossProduct(ToCurrent, ToCandidate).Z;
		if (FMath::Sign(Side) != FMath::Sign(Direction)) { continue; }

		// Prefer candidates with the smallest angular delta from the current target
		// on the requested side — feels like a smooth pan rather than a jump.
		const float Dot = FVector::DotProduct(ToCurrent, ToCandidate);
		const float Score = 1.f - Dot; // 0 = same direction, 2 = opposite
		if (Score < BestScore)
		{
			BestScore = Score;
			Best = Candidate;
		}
	}

	if (Best)
	{
		SetTarget(Best);
	}
}

bool USLLockOnComponent::IsValidCandidate(AActor* Candidate) const
{
	if (!Candidate || Candidate == GetOwner()) { return false; }

	// Drop dead or dying targets. Resolves ASC via direct interface or via
	// PlayerState (player characters host their ASC there).
	UAbilitySystemComponent* ASC = nullptr;
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Candidate))
	{
		ASC = ASI->GetAbilitySystemComponent();
	}
	else if (APawn* Pawn = Cast<APawn>(Candidate))
	{
		if (IAbilitySystemInterface* PSAsi = Cast<IAbilitySystemInterface>(Pawn->GetPlayerState()))
		{
			ASC = PSAsi->GetAbilitySystemComponent();
		}
	}
	if (ASC)
	{
		const FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(SLCombatTags::State_Dead, /*ErrorIfNotFound*/ false);
		if (DeadTag.IsValid() && ASC->HasMatchingGameplayTag(DeadTag))
		{
			return false;
		}
	}

	return true;
}

AActor* USLLockOnComponent::FindBestTargetInCone(AActor* Ignored) const
{
	APawn* Owner = Cast<APawn>(GetOwner());
	if (!Owner) { return nullptr; }

	// Use the camera forward when available — players expect lock-on to look at
	// what they're aiming, not what their character is facing.
	FVector ViewLoc;
	FRotator ViewRot;
	if (Owner->GetController())
	{
		Owner->GetController()->GetPlayerViewPoint(ViewLoc, ViewRot);
	}
	else
	{
		ViewLoc = Owner->GetActorLocation();
		ViewRot = Owner->GetActorRotation();
	}
	const FVector ViewForward = ViewRot.Vector().GetSafeNormal2D();

	TArray<AActor*> ToIgnore;
	ToIgnore.Add(Owner);
	if (Ignored) { ToIgnore.Add(Ignored); }

	TArray<AActor*> Overlapped;
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		Owner->GetActorLocation(),
		LockRange,
		TargetObjectTypes,
		nullptr,
		ToIgnore,
		Overlapped);

	const float CosThreshold = FMath::Cos(FMath::DegreesToRadians(AcquisitionHalfAngle));

	AActor* Best = nullptr;
	// Score = (1 - dot) * Distance — favors centered-and-close. Lower is better.
	float BestScore = TNumericLimits<float>::Max();

	for (AActor* Candidate : Overlapped)
	{
		if (!IsValidCandidate(Candidate)) { continue; }

		const FVector ToTarget = (Candidate->GetActorLocation() - ViewLoc);
		const FVector ToTargetDir = ToTarget.GetSafeNormal2D();
		const float Dot = FVector::DotProduct(ViewForward, ToTargetDir);
		if (Dot < CosThreshold) { continue; }

		const float Score = (1.f - Dot) * 100.f + ToTarget.Size();
		if (Score < BestScore)
		{
			BestScore = Score;
			Best = Candidate;
		}
	}

	return Best;
}

void USLLockOnComponent::SetTarget(AActor* NewTarget)
{
	if (NewTarget == CurrentTarget) { return; }
	CurrentTarget = NewTarget;
	SetComponentTickEnabled(CurrentTarget != nullptr);
	OnLockTargetChanged.Broadcast(CurrentTarget);
}

void USLLockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CurrentTarget) { return; }

	APawn* Owner = Cast<APawn>(GetOwner());
	if (!Owner) { ClearLock(); return; }

	// Auto-release on death / out-of-range / destroyed.
	if (!IsValidCandidate(CurrentTarget))
	{
		ClearLock();
		return;
	}
	const float DistSq = FVector::DistSquared(Owner->GetActorLocation(), CurrentTarget->GetActorLocation());
	if (DistSq > FMath::Square(BreakRange))
	{
		ClearLock();
		return;
	}

	UpdateLockedRotation(DeltaTime);
}

void USLLockOnComponent::UpdateLockedRotation(float DeltaTime)
{
	APawn* Owner = Cast<APawn>(GetOwner());
	if (!Owner) { return; }

	AController* Ctrl = Owner->GetController();
	if (!Ctrl) { return; }

	// Aim from the camera-ish position (eye height) so the pitch tracks chest, not feet.
	const FVector EyeLoc = Owner->GetActorLocation() + FVector(0, 0, Owner->BaseEyeHeight);
	const FVector TargetLoc = CurrentTarget->GetActorLocation();
	const FRotator Desired = (TargetLoc - EyeLoc).Rotation();

	const FRotator Current = Ctrl->GetControlRotation();
	const FRotator Smoothed = FMath::RInterpTo(Current, Desired, DeltaTime, RotationInterpSpeed);
	Ctrl->SetControlRotation(Smoothed);
}
