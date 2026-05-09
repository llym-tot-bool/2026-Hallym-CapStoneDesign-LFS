


#include "SL_ComboManger.h"
#include "Soulslike.h"
#include "SoulslikeCharacter.h"


// Sets default values for this component's properties
USL_ComboManger::USL_ComboManger()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
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

	// ...
}

void USL_ComboManger::OnCharacterInput(FGameplayTag tag_combo)
{
	if (!bIsPlaying) { // initial action start
		bool result = ASC->TryActivateAbilityByClass(combo->GA_list[0]);
		if (!result) { SLDEBUG("fail to activate combo"); return; }
	}
	else { // continue combo action
		if (currentActionIdx == lastActionIdx) {
			currentActionIdx = 0;
		}
		else {
			currentActionIdx++;
		}

		bool result = ASC->TryActivateAbilityByClass(combo->GA_list[currentActionIdx]);
		if (!result) { 
			currentActionIdx--;
			SLDEBUG("fail to continue combo"); 
			return;
		}
	}
}


