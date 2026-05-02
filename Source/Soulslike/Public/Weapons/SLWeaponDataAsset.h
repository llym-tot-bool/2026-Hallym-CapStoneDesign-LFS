#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "SLWeaponTypes.h"
#include "SLWeaponDataAsset.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAnimMontage;
class USkeletalMesh;
class UStaticMesh;

/**
 * Data-driven weapon definition. One asset per weapon; AWeaponBase reads from it on BeginPlay
 * so that designers can tune damage, abilities granted, and visuals without touching C++.
 */
UCLASS(BlueprintType)
class SOULSLIKE_API USLWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ---- Identification --------------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Identity")
	FName WeaponID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Identity")
	FText DisplayName;

	// ---- Classification --------------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Classification")
	ESLWeaponCategory Category = ESLWeaponCategory::Straight;

	/** Gameplay tags describing this weapon (e.g. Weapon.Melee.Sword). Used by GE queries. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Classification")
	FGameplayTagContainer WeaponTags;

	// ---- Visuals / Attachment -------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visuals")
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visuals")
	TSoftObjectPtr<UStaticMesh> StaticMesh;

	/** Socket on the wielder's mesh to attach to when equipped. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visuals")
	FName EquipSocket = TEXT("hand_rSocket");

	/** Socket on the wielder's mesh to attach to when sheathed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visuals")
	FName SheatheSocket = TEXT("spine_03Socket");

	/** Sockets on the weapon mesh itself marking the trace start/end for hit detection. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visuals")
	FName TraceStartSocket = TEXT("TraceStart");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visuals")
	FName TraceEndSocket = TEXT("TraceEnd");

	// ---- Combat Stats ----------------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = "0.0"))
	float BaseDamage = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = "0.0"))
	float StaminaCostLight = 15.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = "0.0"))
	float StaminaCostHeavy = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = "0.0"))
	float PoiseDamage = 10.f;

	/** Radius of the sphere swept between trace sockets for hit detection. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = "0.0"))
	float TraceRadius = 6.f;

	// ---- GAS Integration -------------------------------------------------
	/** Abilities granted to the wielder's ASC while this weapon is equipped. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|GAS")
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

	/** GameplayEffect applied on a confirmed hit against a target with an ASC. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|GAS")
	TSubclassOf<UGameplayEffect> DamageEffect;

	/** Passive GE(s) applied to the wielder while this weapon is equipped (stat modifiers, etc.). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|GAS")
	TArray<TSubclassOf<UGameplayEffect>> EquipEffects;
};
