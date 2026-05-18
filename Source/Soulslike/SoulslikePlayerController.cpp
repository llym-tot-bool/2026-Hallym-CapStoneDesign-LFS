// Copyright Epic Games, Inc. All Rights Reserved.


#include "SoulslikePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Soulslike.h"
#include "UI/SoulslikeHUDWidget.h"
#include "Widgets/Input/SVirtualJoystick.h"

void ASoulslikePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASoulslikePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!SLDA_MeleeCombat) {
		UE_LOG(LogTemp, Warning, TEXT("[SL debug] !!! SetupInputComponent() : SLDA_MeleeCombat is null"));
		return;
	}

	UE_LOG(LogTemp, Display, 
		TEXT("[SL debug] SetupInputComponent() : default weapon type = %s"),
		*SLDA_MeleeCombat->tag_default_weapon.ToString()); // debug

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
				UE_LOG(LogTemp, Display, 
					TEXT("{SL debug] SetupInputComponent() : setup defualt IMC = %s"),
					*CurrentContext->GetFName().ToString());
			}

			if (!SLDA_MeleeCombat) {
				UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! SetupInputComponent() : SLDA_WeaponStyles is null"));
				return;
			}

			for (TObjectPtr<USLDA_WeaponStyle> eachWeaponStyle : SLDA_MeleeCombat->weaponStyle_list) {
				// set default weapon style

				// if weapon tag is same with default weapon tag
				if (eachWeaponStyle->tag_weapon == SLDA_MeleeCombat->tag_default_weapon) {
					tag_currentWeapon = SLDA_MeleeCombat->tag_default_weapon;
					currentIMC = eachWeaponStyle->IMC;

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

void ASoulslikePlayerController::ChangeMeleeControlStyle(FGameplayTag weapon_tag)
{
	// remove older IMC
	UE_LOG(LogTemp, Display,
		TEXT("[SL debug] ChangeMeleeControlStyle() : weapon type = %s"),
		*weapon_tag.ToString());

	if (!IsLocalController()) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		if (currentIMC) { // remove older IMC
			Subsystem->RemoveMappingContext(currentIMC);
		}

		if (!SLDA_MeleeCombat) {
			UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! ChangeMeleeControlStyle() : SLDA_MeleeCombat is null"));
			return;
		}

		for (TObjectPtr<USLDA_WeaponStyle> eachWeaponStyle: SLDA_MeleeCombat->weaponStyle_list) {
			if (weapon_tag == eachWeaponStyle->tag_weapon) {
				tag_currentWeapon = weapon_tag;
				currentIMC = eachWeaponStyle->IMC;
				Subsystem->AddMappingContext(currentIMC, 1);

				UE_LOG(LogTemp, Display,
					TEXT("[SL debug] ChangeMeleeControlStyle() : current weapon type = %s"),
					*tag_currentWeapon.ToString());
				break;
			}
		}
	}
}
