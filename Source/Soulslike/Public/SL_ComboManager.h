

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLDA_MeleeCombat.h"
#include "SoulslikePlayerState.h"
#include "Abilities/SLGA_MeleeSweep.h"

#include "SL_ComboManager.generated.h"

class ASoulslikeCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKE_API USL_ComboManager : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USLDA_WeaponCombo> combo;

	TObjectPtr<UAbilitySystemComponent> ASC;

	bool bIsPlaying = false;
	int currentActionIdx;
	int lastActionIdx;
	ESL_MeleeSweep_State state;

	bool bIsInputBuffered = false;
	TObjectPtr<USLGA_MeleeSweep> currentGA = nullptr;

public:	
	// Sets default values for this component's properties
	USL_ComboManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	void OnCharacterInput(FGameplayTag tag_combo);

	void StartInitialGA();
	void ContinueNextGA();
	void ObserveGA(USLGA_MeleeSweep* targetGA);
	void ObserveQuit();
	void EndCombo();
	
	// bind function for sweep GA's broadcast
	void OnComboInput();
	void OnTranslation();
	void OnRecovery();

};
