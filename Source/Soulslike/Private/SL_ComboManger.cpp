


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
}


// Called every frame
void USL_ComboManger::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USL_ComboManger::OnCharacterInput(FGameplayTag tag_combo)
{
	SLDEBUG("Hello debugging shit %s, %d", *FString("debug"), 12345)
}


