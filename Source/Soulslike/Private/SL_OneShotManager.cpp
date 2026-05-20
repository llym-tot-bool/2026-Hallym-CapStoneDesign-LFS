


#include "SL_OneShotManager.h"
#include "Soulslike.h"
#include "SoulslikeCharacter.h"
#include "Abilities/SLGA_MeleeMultiMontage.h"
#include "SL_HitManager.h"



// Called when the game starts
void USL_OneShotManager::BeginPlay()
{
	Super::BeginPlay();

	SLChar = Cast<ASoulslikeCharacter>(GetOwner()); ensureOrQuit(SLChar);

	ASC = SLChar->GetAbilitySystemComponent();
	ensureOrQuit(ASC);
	ensureOrQuit(combo);
}

void USL_OneShotManager::OnCharacterInput()
{
	if (SLChar->IsFalling()) return;

	if (!bIsPlaying) { // initial action start
		StartGA();
		return;
	}
}



void USL_OneShotManager::StartGA()
{
	ensureOrQuit(!bIsPlaying);

	bool result = ASC->TryActivateAbilityByClass(combo->GA_list[0]);
	if (!result) { SLDEBUG("fail to activate OneShot"); return; }
	SLDEBUG("START OneShot");
	bIsPlaying = true;
	state = ESL_Melee_State::Anticipation;

	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(combo->GA_list[0]); ensureOrQuit(Spec);
	UGameplayAbility* Inst = Spec->GetPrimaryInstance(); ensureOrQuit(Inst);
	USLGA_MeleeMultiMontage* GA = Cast<USLGA_MeleeMultiMontage>(Inst); ensureOrQuit(GA);

	ObserveGA(GA);
}

void USL_OneShotManager::ObserveGA(UGameplayAbility* targetGA)
{
	ensureOrQuit(bIsPlaying);

	currentGA = targetGA;

	if (USLGA_MeleeMultiMontage* convertedGA = Cast<USLGA_MeleeMultiMontage>(currentGA)) {
		convertedGA->delegate_Recovery.AddUObject(this, &USL_OneShotManager::OnRecovery);
		return;
	}

	if (USLGA_MeleeSweep* convertedGA = Cast<USLGA_MeleeSweep>(currentGA)) {
		convertedGA->delegate_Recovery.AddUObject(this, &USL_OneShotManager::OnRecovery);
		return;
	}

}

void USL_OneShotManager::ObserveQuit()
{
	ensureOrQuit(currentGA);

	if (USLGA_MeleeMultiMontage* convertedGA = Cast<USLGA_MeleeMultiMontage>(currentGA)) {
		convertedGA->delegate_Recovery.RemoveAll(this);
		return;
	}

	if (USLGA_MeleeSweep* convertedGA = Cast<USLGA_MeleeSweep>(currentGA)) {
		convertedGA->delegate_Recovery.RemoveAll(this);
		return;
	}
}

void USL_OneShotManager::EndCombo()
{
	ensureOrQuit(bIsPlaying);

	bIsPlaying = false;
	state = ESL_Melee_State::Recovery;
	SLDEBUG("END OneShot")
}

void USL_OneShotManager::OnRecovery()
{
	ensureOrQuit(bIsPlaying);

	ObserveQuit();
	EndCombo();
}
