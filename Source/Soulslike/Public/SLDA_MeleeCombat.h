

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"

#include "SLDA_MeleeCombat.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SOULSLIKE_API USLDA_WeaponCombo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "WeaponCombo|Tag")
	FGameplayTag tag_weapon;
	UPROPERTY(EditAnywhere, Category = "WeaponCombo|Tag")
	FGameplayTag tag_combo;

	UPROPERTY(EditAnywhere, Category = "WeaponCombo|Input")
	TObjectPtr<UInputAction> IA_combo;

	UPROPERTY(EditAnywhere, Category = "WeaponCombo|GA_list")
	TArray<TSubclassOf<UGameplayAbility>> GA_list;
};

UCLASS(Blueprintable)
class SOULSLIKE_API USLDA_WeaponStyle : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "WeaponStyle|Tag")
	FGameplayTag tag_weapon;

	UPROPERTY(EditAnywhere, Category = "WeaponStyle|IMC")
	TObjectPtr<UInputMappingContext> IMC;

	UPROPERTY(EditAnywhere, Category = "WeaponStyle|Combo_list")
	TArray<TObjectPtr<USLDA_WeaponCombo>> combo_list;
};

UCLASS(Blueprintable)
class SOULSLIKE_API USLDA_MeleeCombat : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "MeleeCombat|Default_weapon")
	FGameplayTag tag_default_weapon;

	UPROPERTY(EditAnywhere, Category = "MeleeCombat|Weapon_list")
	TArray<TObjectPtr<USLDA_WeaponStyle>> weaponStyle_list;
};

DECLARE_MULTICAST_DELEGATE(FSL_MeleeSweep_ComboInput)
DECLARE_MULTICAST_DELEGATE(FSL_MeleeSweep_Translation)
DECLARE_MULTICAST_DELEGATE(FSL_MeleeSweep_Recovery)
DECLARE_MULTICAST_DELEGATE(FSL_MeleeSweep_CutMontage)


DECLARE_MULTICAST_DELEGATE(FSL_CharacterMove)
DECLARE_MULTICAST_DELEGATE_OneParam(FSL_CharacterMeleeComboInput, FGameplayTag)


UENUM()
enum class ESL_Melee_State : uint8 {
	Anticipation,
	ComboInput,
	Translation,
	Recovery,
	CutMontage
};

UENUM()
enum class ESL_Melee_TraceState : uint8 {
	none,
	trace
};
