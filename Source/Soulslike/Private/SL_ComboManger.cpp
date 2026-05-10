


#include "SL_ComboManger.h"
#include "Soulslike.h"
#include "SoulslikeCharacter.h"


// Sets default values for this component's properties
USL_ComboManger::USL_ComboManger()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void USL_ComboManger::BeginPlay()
{
	Super::BeginPlay();

	ASoulslikeCharacter* SLChar = Cast<ASoulslikeCharacter>(GetOwner()); ensureOrQuit(SLChar);
	SLChar->delegate_CharacterMeleeComboInput.AddUObject(this, &USL_ComboManger::OnCharacterInput);

	ASC = SLChar->GetAbilitySystemComponent();
	ensureOrQuit(ASC);
	ensureOrQuit(combo);
	
	currentActionIdx = 0;
	lastActionIdx = combo->GA_list.Num() - 1;
	ensureOrQuit(lastActionIdx >= 0);
}


// Called every frame
void USL_ComboManger::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// empty
}

void USL_ComboManger::OnCharacterInput(FGameplayTag tag_combo)
{
	if (!bIsPlaying) { // initial action start
		StartInitialGA();
		return;
	}

	if (currentActionIdx >= lastActionIdx) return;

	switch (state)
	{
	case ESL_MeleeSweep_State::Anticipation:
		return;
	case ESL_MeleeSweep_State::ComboInput:
		bIsInputBuffered = true;
		return;
	case ESL_MeleeSweep_State::Translation:
		ContinueNextGA();
		return;
	case ESL_MeleeSweep_State::Recovery:
		return;
	}
}

void USL_ComboManger::StartInitialGA()
{
	ensureOrQuit(!bIsPlaying);

	currentActionIdx = 0;
	bool result = ASC->TryActivateAbilityByClass(combo->GA_list[currentActionIdx]);
	if (!result) { SLDEBUG("fail to activate combo"); return; }
	SLDEBUG("START combo");
	bIsPlaying = true;
	state = ESL_MeleeSweep_State::Anticipation;

	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(combo->GA_list[currentActionIdx]); ensureOrQuit(Spec);
	UGameplayAbility* Inst = Spec->GetPrimaryInstance(); ensureOrQuit(Inst);
	USLGA_MeleeSweep* sweepGA = Cast<USLGA_MeleeSweep>(Inst); ensureOrQuit(sweepGA);

	ObserveGA(sweepGA);
}

void USL_ComboManger::ContinueNextGA()
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

	state = ESL_MeleeSweep_State::Anticipation;
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

void USL_ComboManger::ObserveGA(USLGA_MeleeSweep* targetGA)
{
	ensureOrQuit(bIsPlaying);

	currentGA = targetGA;

	targetGA->delegate_ComboInput.AddUObject(this, &USL_ComboManger::OnComboInput);
	targetGA->delegate_Translation.AddUObject(this, &USL_ComboManger::OnTranslation);
	targetGA->delegate_Recovery.AddUObject(this, &USL_ComboManger::OnRecovery);
}

void USL_ComboManger::ObserveQuit()
{
	ensureOrQuit(currentGA);

	currentGA->delegate_ComboInput.RemoveAll(this);
	currentGA->delegate_Translation.RemoveAll(this);
	currentGA->delegate_Recovery.RemoveAll(this);
}

void USL_ComboManger::EndCombo()
{
	ensureOrQuit(bIsPlaying);

	bIsPlaying = false;
	state = ESL_MeleeSweep_State::Recovery;
	SLDEBUG("END combo")
}

void USL_ComboManger::OnComboInput()
{
	ensureOrQuit(bIsPlaying);

	state = ESL_MeleeSweep_State::ComboInput;
}

void USL_ComboManger::OnTranslation()
{
	ensureOrQuit(bIsPlaying);

	state = ESL_MeleeSweep_State::Translation;

	if (bIsInputBuffered) {
		ContinueNextGA();
	}
}

void USL_ComboManger::OnRecovery()
{
	ensureOrQuit(bIsPlaying);

	ObserveQuit();
	EndCombo();
}
