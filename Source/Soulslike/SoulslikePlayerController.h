// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SoulslikePlayerState.h"
#include "EnhancedInputComponent.h"

#include "SoulslikePlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */


USTRUCT(BlueprintType)
struct FSLInputActionTagPair {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SL Input")
	UInputAction* InputAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SL Input")
	FGameplayTag GameplayTag;
};

USTRUCT(BlueprintType)
struct FSL_MeleeControlStyle {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "SL Input")
	ESL_WeaponType weapon_type;

	UPROPERTY(EditAnywhere, Category = "SL Input")
	TObjectPtr<UInputMappingContext> IMC;

	UPROPERTY(EditAnywhere, Category = "SL Input")
	TArray<FSLInputActionTagPair> IA_Tag_Pairs;
};

UCLASS(BlueprintType)
class SOULSLIKE_API USLDA_MeleeControlStyles : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESL_WeaponType defaultWeaponType = ESL_WeaponType::none;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSL_MeleeControlStyle> MeleeControlStyles;
};

UCLASS(abstract)
class ASoulslikePlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="SL Input Mappings")
	TArray<TObjectPtr<UInputMappingContext>> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, Category = "SL Input Mappings")
	USLDA_MeleeControlStyles* SLDA_MeleeControlStyles;

	TObjectPtr<UInputMappingContext> currentIMC;
	ESL_WeaponType currentWeaponType;

protected:

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
	void ChangeMeleeControlStyle(ESL_WeaponType weapon_type);

};
