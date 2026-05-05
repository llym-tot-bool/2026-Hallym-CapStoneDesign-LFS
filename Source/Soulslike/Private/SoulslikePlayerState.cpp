


#include "SoulslikePlayerState.h"
#include "GameplayAbilitySpec.h"

ASoulslikePlayerState::ASoulslikePlayerState()
{
	asc = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	AttributeSet = CreateDefaultSubobject<USLCharacterAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ASoulslikePlayerState::GetAbilitySystemComponent() const
{
	return asc;
}

void ASoulslikePlayerState::AddDefaultAbilities()
{
	UE_LOG(LogTemp, Display, 
		TEXT("[SL debug] AddDefaultAbilities() : default weapon type = %s"), 
		*SLDA_MeleeCombat->tag_default_weapon.ToString()); // debug

	if (GetLocalRole() != ROLE_Authority || !asc) { return; }

	currentAbilityHandles.Empty();

	// give default abilities
	for (TObjectPtr<USLDA_WeaponStyle> eachWeaponStyle : SLDA_MeleeCombat->weaponStyle_list) {
		if (eachWeaponStyle->tag_weapon == SLDA_MeleeCombat->tag_default_weapon) {
			for (TObjectPtr<USLDA_WeaponCombo> eachWeaponCombo: eachWeaponStyle->combo_list) {
				for (TSubclassOf<UGameplayAbility> eachGA : eachWeaponCombo->GA_list) {
					currentAbilityHandles.Add(asc->GiveAbility(FGameplayAbilitySpec(eachGA, 1)));
				}

				UE_LOG(LogTemp, Display, 
					TEXT("[SL debug] AddDefaultAbilities() : add combo = %s"),
					*eachWeaponCombo->tag_combo.ToString());
			}
			break;
		}
	}

	if (currentAbilityHandles.Num() == 0) {
		UE_LOG(LogTemp, Display, 
			TEXT("[SL debug] !!! AddDefaultAbilities() : current ability count is 0. may be some problem?"));
	}
	else {
		UE_LOG(LogTemp, Display, 
			TEXT("[SL debug] AddDefaultAbilities() : current ability count is %d"), currentAbilityHandles.Num());
	}
}

void ASoulslikePlayerState::ChangeMeleeStyle(FGameplayTag weapon_tag)
{
	UE_LOG(LogTemp, Display, 
		TEXT("[SL debug] ChangeMeleeStyle() : %s"),
		*weapon_tag.ToString());

	for (FGameplayAbilitySpecHandle& abilityHandle : currentAbilityHandles) {
		asc->ClearAbility(abilityHandle);
	}

	currentAbilityHandles.Empty();

	for (TObjectPtr<USLDA_WeaponStyle> eachWeaponStyle : SLDA_MeleeCombat->weaponStyle_list) {
		if (eachWeaponStyle->tag_weapon == weapon_tag) {
			for (TObjectPtr<USLDA_WeaponCombo> eachWeaponCombo : eachWeaponStyle->combo_list) {
				for (TSubclassOf<UGameplayAbility> eachGA : eachWeaponCombo->GA_list) {
					currentAbilityHandles.Add(asc->GiveAbility(FGameplayAbilitySpec(eachGA, 1)));
				}
			}
			break;
		}
	}
}
