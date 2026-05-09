

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLDA_MeleeCombat.h"
#include "SoulslikePlayerState.h"

#include "SL_ComboManger.generated.h"

class ASoulslikeCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKE_API USL_ComboManger : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USLDA_WeaponCombo> combo;

	TObjectPtr<UAbilitySystemComponent> ASC;

	bool bIsPlaying = false;
	int currentActionIdx;
	int lastActionIdx;

public:	
	// Sets default values for this component's properties
	USL_ComboManger();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnCharacterInput(FGameplayTag tag_combo);
	
};
