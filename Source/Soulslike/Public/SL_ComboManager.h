

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

public:
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USLDA_WeaponCombo> combo;

protected:
	TObjectPtr<UAbilitySystemComponent> ASC;
	TObjectPtr<ASoulslikeCharacter> SLChar;

	bool bIsPlaying = false;
	int currentActionIdx;
	int lastActionIdx;
	ESL_Melee_State state;

	bool bIsInputBuffered = false;
	TObjectPtr<USLGA_MeleeSweep> currentGA = nullptr;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	void OnCharacterInput();

protected:
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
