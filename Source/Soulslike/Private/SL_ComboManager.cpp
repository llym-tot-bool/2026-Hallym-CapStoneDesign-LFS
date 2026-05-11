


#include "SL_ComboManager.h"
#include "Soulslike.h"
#include "SoulslikeCharacter.h"
#include "Abilities/SLGA_MeleeMultiMontage.h"


// Called when the game starts
void USL_ComboManager::BeginPlay()
{
	Super::BeginPlay();

	SLChar = Cast<ASoulslikeCharacter>(GetOwner()); ensureOrQuit(SLChar);

	ASC = SLChar->GetAbilitySystemComponent();
	ensureOrQuit(ASC);
	ensureOrQuit(combo);
	
	currentActionIdx = 0;
	lastActionIdx = combo->GA_list.Num() - 1;
	ensureOrQuit(lastActionIdx >= 0);
}


void USL_ComboManager::OnCharacterInput()
{
	if (SLChar->IsFalling()) return;

	if (!bIsPlaying) { // initial action start
		StartInitialGA();
		return;
	}

	if (currentActionIdx >= lastActionIdx) return;

	switch (state)
	{
	case ESL_Melee_State::Anticipation:
		return;
	case ESL_Melee_State::ComboInput:
		bIsInputBuffered = true;
		return;
	case ESL_Melee_State::Translation:
		ContinueNextGA();
		return;
	case ESL_Melee_State::Recovery:
		return;
	}
}

void USL_ComboManager::StartInitialGA()
{
	ensureOrQuit(!bIsPlaying);

	currentActionIdx = 0;
	bool result = ASC->TryActivateAbilityByClass(combo->GA_list[currentActionIdx]);
	if (!result) { SLDEBUG("fail to activate combo"); return; }
	SLDEBUG("START combo");
	bIsPlaying = true;
	state = ESL_Melee_State::Anticipation;

	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(combo->GA_list[currentActionIdx]); ensureOrQuit(Spec);
	UGameplayAbility* Inst = Spec->GetPrimaryInstance(); ensureOrQuit(Inst);
	USLGA_MeleeSweep* sweepGA = Cast<USLGA_MeleeSweep>(Inst); ensureOrQuit(sweepGA);

	ObserveGA(sweepGA);
}

void USL_ComboManager::ContinueNextGA()
{
	ensureOrQuit(currentGA);
	ensureOrQuit(bIsPlaying);

	bIsInputBuffered = false;

	int nextActionIdx = currentActionIdx + 1;
	FGameplayAbilitySpec* nextGASpec = ASC->FindAbilitySpecFromClass(combo->GA_list[nextActionIdx]); ensureOrQuit(nextGASpec);
	FGameplayTagContainer FailureTags;

	bool bCanActivate = nextGASpec->Ability->CanActivateAbility(
		nextGASpec->Handle,
		ASC->AbilityActorInfo.Get(),
		nullptr, // Current Tags
		nullptr, // Target Tags
		&FailureTags
	);

	if (!bCanActivate)
	{
		SLDEBUG("fail to continue combo");
		return;
	}

	state = ESL_Melee_State::Anticipation;
	ObserveQuit();
	currentGA->InterruptAsCombo();
	currentGA = nullptr;

	SLDEBUG("this is last action for combo");
	currentActionIdx = nextActionIdx;
	bool result = ASC->TryActivateAbilityByClass(combo->GA_list[currentActionIdx]); ensureOrQuit(result);
	FGameplayAbilitySpec* spec = ASC->FindAbilitySpecFromClass(combo->GA_list[currentActionIdx]); ensureOrQuit(spec);
	UGameplayAbility* inst = spec->GetPrimaryInstance(); ensureOrQuit(inst);
	USLGA_MeleeSweep* sweepGA = Cast<USLGA_MeleeSweep>(inst); ensureOrQuit(sweepGA);

	ObserveGA(sweepGA);
}


void USL_ComboManager::ObserveGA(USLGA_MeleeSweep* targetGA)
{
	ensureOrQuit(bIsPlaying);

	currentGA = targetGA;

	targetGA->delegate_ComboInput.AddUObject(this, &USL_ComboManager::OnComboInput);
	targetGA->delegate_Translation.AddUObject(this, &USL_ComboManager::OnTranslation);
	targetGA->delegate_Recovery.AddUObject(this, &USL_ComboManager::OnRecovery);
}

void USL_ComboManager::ObserveQuit()
{
	ensureOrQuit(currentGA);

	currentGA->delegate_ComboInput.RemoveAll(this);
	currentGA->delegate_Translation.RemoveAll(this);
	currentGA->delegate_Recovery.RemoveAll(this);
}

void USL_ComboManager::EndCombo()
{
	ensureOrQuit(bIsPlaying);

	bIsPlaying = false;
	state = ESL_Melee_State::Recovery;
	SLDEBUG("END combo")
}

void USL_ComboManager::OnComboInput()
{
	ensureOrQuit(bIsPlaying);

	state = ESL_Melee_State::ComboInput;
}

void USL_ComboManager::OnTranslation()
{
	ensureOrQuit(bIsPlaying);

	state = ESL_Melee_State::Translation;

	if (bIsInputBuffered) {
		ContinueNextGA();
	}
}

void USL_ComboManager::OnRecovery()
{
	ensureOrQuit(bIsPlaying);

	ObserveQuit();
	EndCombo();
}
