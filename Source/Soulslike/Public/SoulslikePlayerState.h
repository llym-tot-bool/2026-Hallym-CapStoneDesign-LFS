

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "SLCharacterAttributeSet.h"
#include "SLDA_MeleeCombat.h"

#include "SoulslikePlayerState.generated.h"

/**
 * 
 */

UCLASS()
class SOULSLIKE_API ASoulslikePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ASoulslikePlayerState();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void AddDefaultAbilities();

	void ChangeMeleeStyle(FGameplayTag weapon_tag);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SL GAS")
	class UAbilitySystemComponent* asc;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SL GAS")
	class USLCharacterAttributeSet* AttributeSet;

	UPROPERTY(EditAnywhere, Category = "SL GAS")
	TObjectPtr<USLDA_MeleeCombat> SLDA_MeleeCombat;

	TArray<FGameplayAbilitySpecHandle> currentAbilityHandles;
};
