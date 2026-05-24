


#include "SL_CharDeathManager.h"
#include "Soulslike.h"
#include "SoulslikeCharacter.h"

void USL_CharDeathManager::BeginPlay()
{
	Super::BeginPlay();

	SLChar = Cast<ASoulslikeCharacter>(GetOwner()); ensureOrQuit(SLChar);

	ASC = SLChar->GetAbilitySystemComponent();
	ensureOrQuit(ASC);
	ensureOrQuit(onDeathGA_class);
}

void USL_CharDeathManager::OnDeath()
{
	StartGA();
}

void USL_CharDeathManager::StartGA()
{
	ensureOrQuit(!bIsPlaying);

	bool result = ASC->TryActivateAbilityByClass(onDeathGA_class);
	if (!result) { SLDEBUG("fail to activate DeathGA"); return; }
	SLDEBUG("START DeathGA");
	bIsPlaying = true;

	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(onDeathGA_class); ensureOrQuit(Spec);
	UGameplayAbility* Inst = Spec->GetPrimaryInstance(); ensureOrQuit(Inst);
	USLGA_CharDeath* GA = Cast<USLGA_CharDeath>(Inst); ensureOrQuit(GA);

}

void USL_CharDeathManager::EndDeathGA()
{
	ensureOrQuit(bIsPlaying);

	bIsPlaying = false;
	SLDEBUG("END OnHitGA")
}