


#include "SL_OneShotManager.h"
#include "Soulslike.h"
#include "SoulslikeCharacter.h"
#include "Abilities/SLGA_MeleeMultiMontage.h"



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

void USL_OneShotManager::ObserveGA(USLGA_MeleeMultiMontage* targetGA)
{
	ensureOrQuit(bIsPlaying);

	currentGA = targetGA;

	targetGA->delegate_Recovery.AddUObject(this, &USL_OneShotManager::OnRecovery);
}

void USL_OneShotManager::ObserveQuit()
{
	ensureOrQuit(currentGA);

	currentGA->delegate_Recovery.RemoveAll(this);
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
