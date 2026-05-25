

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLDA_MeleeCombat.h"
#include "Abilities/SLGA_OnHit.h"

#include "SL_HitManager.generated.h"

class ASoulslikeCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SOULSLIKE_API USL_HitManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "SL")
	TSubclassOf<USLGA_OnHit> onhitGA_class;

	UPROPERTY(EditAnywhere, Category = "SL")
	TObjectPtr<USoundBase> HitSound;

protected:
	TObjectPtr<UAbilitySystemComponent> ASC;
	TObjectPtr<ASoulslikeCharacter> SLChar;

	bool bIsPlaying = false;
	ESL_Melee_State state;

	TObjectPtr<USLGA_OnHit> onhitGA = nullptr;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void OnHit();

protected:
	void StartGA();
	void ObserveGA(USLGA_OnHit* targetGA);
	void ObserveQuit();
	void EndHitGA();

	// bind function for sweep GA's broadcast
	void OnRecovery();
};
