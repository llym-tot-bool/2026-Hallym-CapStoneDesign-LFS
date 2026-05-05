// Copyright Epic Games, Inc. All Rights Reserved.


#include "SoulslikePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Soulslike.h"
#include "Widgets/Input/SVirtualJoystick.h"

void ASoulslikePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASoulslikePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UE_LOG(LogTemp, Display, 
		TEXT("[SL debug] SetupInputComponent() : default weapon type = %s"),
		*UEnum::GetValueAsString(SLDA_MeleeControlStyles->defaultWeaponType)); // debug

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
				UE_LOG(LogTemp, Display, 
					TEXT("{SL debug] SetupInputComponent() : setup defualt IMC = %s"),
					*CurrentContext->GetFName().ToString());
			}

			if (!SLDA_MeleeControlStyles) {
				UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! SetupInputComponent() : SLDA_MeleeControlStyles is null"));
				return;
			}

			for (FSL_MeleeControlStyle& eachCotnrolStyle : SLDA_MeleeControlStyles->MeleeControlStyles) {
				if (SLDA_MeleeControlStyles->defaultWeaponType == eachCotnrolStyle.weapon_type) {
					currentWeaponType = eachCotnrolStyle.weapon_type;
					currentIMC = eachCotnrolStyle.IMC;
					Subsystem->AddMappingContext(currentIMC, 1);

					UE_LOG(LogTemp, Display, 
						TEXT("[SL debug] SetupInputComponent() : current weapon IMC = %s"),
						*currentIMC->GetFName().ToString());
					break;
				}
			}

			if (!currentIMC) {
				UE_LOG(LogTemp, Display,
					TEXT("[SL debug] !!! SetupInputComponent() : setup defualt weapon IMC failed."));
			}
		}
	}
}

void ASoulslikePlayerController::ChangeMeleeControlStyle(ESL_WeaponType weapon_type)
{
	UE_LOG(LogTemp, Display, 
		TEXT("[SL debug] ChangeMeleeControlStyle() : weapon type = %s"),
		*UEnum::GetValueAsString(weapon_type)); // debug

	if (IsLocalPlayerController()) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
			Subsystem->RemoveMappingContext(currentIMC);

			if (!SLDA_MeleeControlStyles) {
				UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! ChangeMeleeControlStyle() : SLDA_MeleeControlStyles is null"));
				return;
			}
			
			for (FSL_MeleeControlStyle& eachCotnrolStyle : SLDA_MeleeControlStyles->MeleeControlStyles) {
				if (weapon_type == eachCotnrolStyle.weapon_type) {
					currentWeaponType = weapon_type;
					currentIMC = eachCotnrolStyle.IMC;
					Subsystem->AddMappingContext(eachCotnrolStyle.IMC, 1);

					UE_LOG(LogTemp, Display, 
						TEXT("[SL debug] ChangeMeleeControlStyle() : current weapon type = %s"),
						*UEnum::GetValueAsString(currentWeaponType));
				}
				break;
			}
		}
	}
}		
