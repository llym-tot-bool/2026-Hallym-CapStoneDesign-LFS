// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SoulslikePlayerState.h"
#include "EnhancedInputComponent.h"
#include "SLDA_MeleeCombat.h"

#include "SoulslikePlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class USoulslikeHUDWidget;

UCLASS(abstract)
class ASoulslikePlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="SL Input Mappings")
	TArray<TObjectPtr<UInputMappingContext>> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, Category = "SL Input Mappings")
	TObjectPtr<USLDA_MeleeCombat> SLDA_MeleeCombat;

	TObjectPtr<UInputMappingContext> currentIMC;
	FGameplayTag tag_currentWeapon;

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
	void ChangeMeleeControlStyle(FGameplayTag weapon_tag);

};
