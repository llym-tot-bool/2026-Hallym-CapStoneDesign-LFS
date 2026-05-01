// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SoulslikePlayerState.h"

#include "SoulslikePlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */

USTRUCT(BlueprintType)
struct FSL_MeleeControlStyle {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input")
	ESL_WeaponType weapon_type;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC;
};

UCLASS(abstract)
class ASoulslikePlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<TObjectPtr<UInputMappingContext>> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	TArray<FSL_MeleeControlStyle> MeleeControlStyles;

	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	ESL_WeaponType defaultWeaponType = ESL_WeaponType::none;

	TObjectPtr<UInputMappingContext> currentIMC;

protected:

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
	void ChangeMeleeControlStyle(ESL_WeaponType weapon_type);

};
