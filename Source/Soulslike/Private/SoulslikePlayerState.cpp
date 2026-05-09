


#include "SoulslikePlayerState.h"
#include "GameplayAbilitySpec.h"

ASoulslikePlayerState::ASoulslikePlayerState()
{
	asc = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	asc->SetIsReplicated(true);
	asc->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<USLCharacterAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ASoulslikePlayerState::GetAbilitySystemComponent() const
{
	return asc;
}

void ASoulslikePlayerState::AddDefaultAbilities()
{
	if (!SLDA_MeleeCombat) {
		UE_LOG(LogTemp, Warning, TEXT("[SL debug] !!! AddDefaultAbilities() : SLDA_MeleeCombat is null"));
		return;
	}

	UE_LOG(LogTemp, Display, 
		TEXT("[SL debug] AddDefaultAbilities() : default weapon type = %s"), 
		*SLDA_MeleeCombat->tag_default_weapon.ToString()); // debug

	if (GetLocalRole() != ROLE_Authority || !asc) { return; }

	currentAbilityHandles.Empty();

	// give default abilities
	ChangeMeleeStyle(SLDA_MeleeCombat->tag_default_weapon);

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
	if (!SLDA_MeleeCombat || !asc) return;

	UE_LOG(LogTemp, Display, 
		TEXT("[SL debug] ChangeMeleeStyle() : %s"),
		*weapon_tag.ToString());

	for (FGameplayAbilitySpecHandle& abilityHandle : currentAbilityHandles) {
		asc->ClearAbility(abilityHandle);
	}

	currentAbilityHandles.Empty();

	for (TObjectPtr<USLDA_WeaponStyle> eachWeaponStyle : SLDA_MeleeCombat->weaponStyle_list) {
		if (eachWeaponStyle && eachWeaponStyle->tag_weapon == weapon_tag) {
			for (TObjectPtr<USLDA_WeaponCombo> eachWeaponCombo : eachWeaponStyle->combo_list) {
				if (eachWeaponCombo) {
					//currentAbilityHandles.Add(asc->GiveAbility(FGameplayAbilitySpec(eachWeaponCombo->GA_Combo, 1)));

					for (TSubclassOf<UGameplayAbility> eachGA : eachWeaponCombo->GA_list) {
						if (eachGA) {
							currentAbilityHandles.Add(asc->GiveAbility(FGameplayAbilitySpec(eachGA, 1)));
						}
					}
				}
			}
			break;
		}
	}
}
