#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLLockOnComponent.generated.h"

class APawn;
class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLOnLockTargetChanged, AActor*, NewTarget);

/**
 * Soulslike target lock-on. Attached to the player character.
 *
 * Toggle: ToggleLockOn() picks the best enemy in a forward cone within
 *   LockRange and binds the controller's view to face them every tick.
 *   Calling ToggleLockOn() again while locked clears the lock.
 *
 * Auto-release: the lock drops when the target moves beyond BreakRange,
 *   when the target gains State.Dead, or when the target is destroyed.
 *
 * Target switch: SwitchTarget(direction) cycles to the next valid candidate
 *   on either side of the current target — wire it to right-stick deltas
 *   or shoulder buttons.
 */
UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class SOULSLIKE_API USLLockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USLLockOnComponent();

	/** Toggle the lock on / off. If off, finds the best target in front and locks. */
	UFUNCTION(BlueprintCallable, Category = "Lock On")
	void ToggleLockOn();

	/** Force-clear the current lock. */
	UFUNCTION(BlueprintCallable, Category = "Lock On")
	void ClearLock();

	/**
	 * Switch to the next valid target. Direction > 0 looks right of the current
	 * target, Direction < 0 looks left. No-op if not currently locked.
	 */
	UFUNCTION(BlueprintCallable, Category = "Lock On")
	void SwitchTarget(float Direction);

	UFUNCTION(BlueprintPure, Category = "Lock On")
	bool IsLocked() const { return CurrentTarget != nullptr; }

	UFUNCTION(BlueprintPure, Category = "Lock On")
	AActor* GetLockedTarget() const { return CurrentTarget; }

	/** Fires whenever the lock target changes — including unlock (passes nullptr). */
	UPROPERTY(BlueprintAssignable, Category = "Lock On")
	FSLOnLockTargetChanged OnLockTargetChanged;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Run a sphere overlap to gather candidate enemies, then score and pick the best. */
	AActor* FindBestTargetInCone(AActor* Ignored = nullptr) const;

	/** Returns true if Candidate is alive, on the visible side, and (for switch) on the requested side. */
	bool IsValidCandidate(AActor* Candidate) const;

	/** Update controller rotation each tick so the camera tracks CurrentTarget. */
	void UpdateLockedRotation(float DeltaTime);

	void SetTarget(AActor* NewTarget);

protected:
	/** Maximum acquisition range when toggling on. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lock On|Tuning", meta = (ClampMin = "0.0"))
	float LockRange = 1500.f;

	/** Once locked, target is auto-released past this distance. Should be > LockRange. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lock On|Tuning", meta = (ClampMin = "0.0"))
	float BreakRange = 2000.f;

	/** Acquisition cone half-angle (degrees) measured from the camera forward. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lock On|Tuning", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float AcquisitionHalfAngle = 60.f;

	/** Speed (deg/sec) at which the controller yaw eases toward the target. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lock On|Tuning", meta = (ClampMin = "0.0"))
	float RotationInterpSpeed = 8.f;

	/**
	 * Object types considered for lock-on overlap. Defaults are populated to Pawn
	 * in BeginPlay if the array is left empty in the BP defaults.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lock On|Tuning")
	TArray<TEnumAsByte<EObjectTypeQuery>> TargetObjectTypes;

private:
	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget;
};
