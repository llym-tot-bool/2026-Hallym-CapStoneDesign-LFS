
#include "ANS/SLAN_PlaySound.h"

#include "Kismet/GameplayStatics.h"
#include "Soulslike.h"

void USLAN_PlaySound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (Sound) {
		UGameplayStatics::PlaySoundAtLocation(MeshComp, Sound, MeshComp->GetSocketLocation(socket_for_sound));
	}
	else {
		SLDEBUG("not sound for SLAN play sound");
	}
}
