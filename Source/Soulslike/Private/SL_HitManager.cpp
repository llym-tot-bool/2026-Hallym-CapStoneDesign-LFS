


#include "SL_HitManager.h"
#include "Soulslike.h"
#include "SoulslikeCharacter.h"
#include "Kismet/GameplayStatics.h"


// Called when the game starts
void USL_HitManager::BeginPlay()
{
	Super::BeginPlay();

	SLChar = Cast<ASoulslikeCharacter>(GetOwner()); ensureOrQuit(SLChar);

	ASC = SLChar->GetAbilitySystemComponent();
	ensureOrQuit(ASC);
	ensureOrQuit(onhitGA_class);

	bIsPlaying = false;
}

void USL_HitManager::OnHit()
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(SLChar, HitSound, SLChar->K2_GetActorLocation());
	}
	StartGA();
}

void USL_HitManager::StartGA()
{

	bool result = ASC->TryActivateAbilityByClass(onhitGA_class);
	if (!result) { SLDEBUG("fail to activate HitGA"); return; }
	SLDEBUG("START HitGA");
	bIsPlaying = true;
	state = ESL_Melee_State::Anticipation;

	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(onhitGA_class); ensureOrQuit(Spec);
	UGameplayAbility* Inst = Spec->GetPrimaryInstance(); ensureOrQuit(Inst);
	USLGA_OnHit* GA = Cast<USLGA_OnHit>(Inst); ensureOrQuit(GA);

	ObserveGA(GA);
}

void USL_HitManager::ObserveGA(USLGA_OnHit* targetGA)
{
	ensureOrQuit(bIsPlaying);

	onhitGA = targetGA;
	onhitGA->delegate_Recovery.AddUObject(this, &USL_HitManager::OnRecovery);
}

void USL_HitManager::ObserveQuit()
{
	ensureOrQuit(onhitGA);

	onhitGA->delegate_Recovery.RemoveAll(this);
	onhitGA = nullptr;
}

void USL_HitManager::EndHitGA()
{
	ensureOrQuit(bIsPlaying);

	bIsPlaying = false;
	state = ESL_Melee_State::Recovery;
	SLDEBUG("END OnHitGA")
}

void USL_HitManager::OnRecovery()
{
	ensureOrQuit(bIsPlaying);

	ObserveQuit();
	EndHitGA();
}
