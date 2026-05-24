

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLDA_MeleeCombat.h"
#include "Abilities/SLGA_CharDeath.h"

#include "SL_CharDeathManager.generated.h"

class ASoulslikeCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULSLIKE_API USL_CharDeathManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "SL|OnDeath")
	TSubclassOf<USLGA_CharDeath> onDeathGA_class;

protected:
	TObjectPtr<UAbilitySystemComponent> ASC;
	TObjectPtr<ASoulslikeCharacter> SLChar;
	
	bool bIsPlaying = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void OnDeath();

protected:
	void StartGA();
	void EndDeathGA();
	
};
