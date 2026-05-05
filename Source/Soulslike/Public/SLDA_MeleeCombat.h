

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
	UPROPERTY(EditAnywhere, Category = "WeaponCombo|Tag")
	FGameplayTag tag_inputAsCombo;
	UPROPERTY(EditAnywhere, Category = "WeaponCombo|Tag")
	FGameplayTag tag_comboGrant;
	UPROPERTY(EditAnywhere, Category = "WeaponCombo|Tag")
	FGameplayTag tag_tryActivateCombo;

	UPROPERTY(EditAnywhere, Category = "WeaponCombo|IA")
	TObjectPtr<UInputAction> IA_combo;

	UPROPERTY(EditAnywhere, Category = "WeaponCombo|Initial_activation")
	FGameplayTag tag_initialActivation;

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
