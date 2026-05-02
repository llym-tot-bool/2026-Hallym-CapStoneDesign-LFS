

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "SLCharacterAttributeSet.h"

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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	class UAbilitySystemComponent* asc;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	class USLCharacterAttributeSet* AttributeSet;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<class UGameplayAbility>> defaultAbilities;
};
