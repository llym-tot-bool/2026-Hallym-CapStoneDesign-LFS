#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "SLWeaponBase.generated.h"

class USkeletalMeshComponent;
class UStaticMeshComponent;
class UAbilitySystemComponent;
class UGameplayEffect;
class USLWeaponDataAsset;
class ACharacter;

/** Broadcast when the weapon's active sweep hits a new actor during an attack window. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FSLWeaponHitSignature,
	AActor*, HitActor,
	const FHitResult&, HitResult);

/**
 * Base weapon actor. Spawned and owned by a character, attached to a socket when equipped.
 *
 * Hit detection is *not* continuous: gameplay abilities (or anim notifies) call
 * StartHitDetection()/StopHitDetection() to open a swing window, during which the
 * weapon performs capsule sweeps between its trace sockets every Tick and fires
 * OnWeaponHit for any actor it strikes (each actor is only reported once per window).
 *
 * Damage and effects are applied by the owning ability / caller via GAS — this class
 * only reports hits and exposes the data asset for cost/damage values.
 */
UCLASS(Blueprintable)
class SOULSLIKE_API ASLWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	ASLWeaponBase();

	// ---- Equip / Unequip -------------------------------------------------
	/** Attaches this weapon to OwnerCharacter at the data asset's EquipSocket and grants abilities. */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Equip(ACharacter* NewOwnerCharacter);

	/** Detaches, removes granted abilities/effects. Does not destroy the weapon. */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Unequip();

	/** Swap attachment to the sheathe socket (e.g. on back). Does not remove abilities. */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Sheathe();

	/** Swap attachment back to the equip socket. */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Draw();

	// ---- Hit Detection ---------------------------------------------------
	/** Open the attack hit window. Call from an ability or AnimNotify at windup end. */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Combat")
	virtual void StartHitDetection();

	/** Close the hit window and clear the per-swing actor list. */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Combat")
	virtual void StopHitDetection();

	UFUNCTION(BlueprintPure, Category = "Weapon|Combat")
	bool IsHitDetectionActive() const { return bHitDetectionActive; }

	// ---- Accessors -------------------------------------------------------
	UFUNCTION(BlueprintPure, Category = "Weapon")
	USLWeaponDataAsset* GetWeaponData() const { return WeaponData; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	ACharacter* GetOwnerCharacter() const { return OwnerCharacter; }

	/** Fired on the authority whenever a new actor is hit during the active window. */
	UPROPERTY(BlueprintAssignable, Category = "Weapon|Combat")
	FSLWeaponHitSignature OnWeaponHit;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Resolve which mesh component to use for socket queries (skeletal preferred). */
	USceneComponent* GetActiveMeshComponent() const;

	/** Pull the owner's ASC. Assumes owner implements IAbilitySystemInterface (typical via PlayerState). */
	UAbilitySystemComponent* GetOwnerASC() const;

	/** Apply WeaponData->EquipEffects to the owner's ASC. */
	void ApplyEquipEffects();

	/** Grant WeaponData->GrantedAbilities. Authority only. */
	void GrantAbilities();

	/** Remove abilities / effects granted in Equip. Authority only. */
	void ClearGrantedGAS();

	/** Perform the sweep for this frame and broadcast hits. */
	void PerformHitTrace();

	/**
	 * Apply the configured damage GE to Target's ASC.
	 *
	 * Resolves the GE class in this priority order:
	 *   1. WeaponData->DamageEffect, if set
	 *   2. DefaultDamageEffectClass (USLGE_WeaponDamage by default)
	 *
	 * Damage magnitude is supplied via SetByCaller("Data.Damage.Base") using
	 * WeaponData->BaseDamage. Authority-only: damage application is server-side.
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Combat")
	virtual void ApplyDamageToTarget(AActor* Target, const FHitResult& Hit);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;

	/** The authoring data asset. Assign in BP defaults or via a spawning factory. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<USLWeaponDataAsset> WeaponData;

	/** Owner character; set in Equip(). Not replicated — set again on each machine. */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<ACharacter> OwnerCharacter;

	/** Collision channel the hit sweep queries against. */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Combat")
	TEnumAsByte<ECollisionChannel> HitTraceChannel = ECC_Pawn;

	/** If true the weapon traces every tick while active; else the caller must poll PerformHitTrace. */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Combat")
	bool bAutoTickHitTrace = true;

	/**
	 * Fallback damage GE used when WeaponData->DamageEffect is unset. Defaults to
	 * USLGE_WeaponDamage; can be overridden in BP for project-wide custom damage rules.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Combat")
	TSubclassOf<UGameplayEffect> DefaultDamageEffectClass;

	/**
	 * If true (default), PerformHitTrace will automatically call ApplyDamageToTarget on
	 * each newly hit actor right after broadcasting OnWeaponHit. Disable this when an
	 * ability wants to gate damage with extra logic (e.g. parry checks) and apply it manually.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Combat")
	bool bAutoApplyDamageOnHit = true;

private:
	bool bHitDetectionActive = false;

	/** Actors already reported during the current attack window — cleared on StopHitDetection. */
	UPROPERTY(Transient)
	TSet<TObjectPtr<AActor>> HitActorsThisSwing;

	/** Last trace-start location, used to build a continuous sweep between frames. */
	FVector LastTraceStart = FVector::ZeroVector;
	FVector LastTraceEnd = FVector::ZeroVector;
	bool bHasPrevTrace = false;

	/** Handles for cleanup on Unequip. */
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;
	TArray<FActiveGameplayEffectHandle> AppliedEquipEffectHandles;
};
