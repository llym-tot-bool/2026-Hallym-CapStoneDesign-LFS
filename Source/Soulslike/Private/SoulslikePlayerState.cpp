


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
		*UEnum::GetValueAsString(defaultWeaponType)); // debug

	if (GetLocalRole() != ROLE_Authority || !asc) { return; }

	currentAbilityHandles.Empty();

	for (const FSL_MeleeStyle& eachMeleeStyle : MeleeStyles) {
		if (eachMeleeStyle.weapon_type == defaultWeaponType) {
			for (const TSubclassOf<UGameplayAbility>& eachAbility : eachMeleeStyle.abilities) {
				currentAbilityHandles.Add(asc->GiveAbility(FGameplayAbilitySpec(eachAbility, 1)));
			}
			break;
		}
	}
}

void ASoulslikePlayerState::ChangeMeleeStyle(ESL_WeaponType weapon_type)
{
	UE_LOG(LogTemp, Display, 
		TEXT("[SL debug] ChangeMeleeStyle() : %s"),
		*UEnum::GetValueAsString(weapon_type)); // debug

	for (FGameplayAbilitySpecHandle& abilityHandle : currentAbilityHandles) {
		asc->ClearAbility(abilityHandle);
	}

	currentAbilityHandles.Empty();

	for (const FSL_MeleeStyle& eachMeleeStyle : MeleeStyles) {
		if (eachMeleeStyle.weapon_type == weapon_type) {
			for (const TSubclassOf<UGameplayAbility>& eachAbility : eachMeleeStyle.abilities) {
				currentAbilityHandles.Add(asc->GiveAbility(FGameplayAbilitySpec(eachAbility, 1)));
			}
			break;
		}
	}
}
