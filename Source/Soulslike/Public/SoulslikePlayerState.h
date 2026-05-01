

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

UENUM(BlueprintType)
enum class ESL_WeaponType : uint8 {
	none UMETA(DisplayName = "none"),
	katana UMETA(DisplayName = "Katana")
};

USTRUCT(BlueprintType)
struct FSL_MeleeStyle {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "GAS")
	ESL_WeaponType weapon_type;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> abilities;
};

UCLASS()
class SOULSLIKE_API ASoulslikePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ASoulslikePlayerState();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void AddDefaultAbilities();

	void ChangeMeleeStyle(ESL_WeaponType weapon_type);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	class UAbilitySystemComponent* asc;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	class USLCharacterAttributeSet* AttributeSet;

	UPROPERTY(EditAnywhere, Category = "GAS")
	ESL_WeaponType defaultWeaponType = ESL_WeaponType::none;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TArray<FSL_MeleeStyle> MeleeStyles;

	TArray<FGameplayAbilitySpecHandle> currentAbilityHandles;
};
