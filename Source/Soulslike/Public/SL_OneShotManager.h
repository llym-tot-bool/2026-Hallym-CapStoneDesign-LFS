

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLDA_MeleeCombat.h"
#include "SoulslikePlayerState.h"
#include "Abilities/SLGA_MeleeSweep.h"
#include "Abilities/SLGA_MeleeMultiMontage.h"


#include "SL_OneShotManager.generated.h"

class ASoulslikeCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKE_API USL_OneShotManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USLDA_WeaponCombo> combo;

protected:
	TObjectPtr<UAbilitySystemComponent> ASC;
	TObjectPtr<ASoulslikeCharacter> SLChar;


	bool bIsPlaying = false;
	ESL_Melee_State state;

	TObjectPtr<USLGA_MeleeMultiMontage> currentGA = nullptr;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void OnCharacterInput();

protected:
	void StartGA();
	void ObserveGA(USLGA_MeleeMultiMontage* targetGA);
	void ObserveQuit();
	void EndCombo();

	// bind function for sweep GA's broadcast
	void OnRecovery();
};
