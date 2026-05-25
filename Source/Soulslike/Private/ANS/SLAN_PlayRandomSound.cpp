

#include "ANS/SLAN_PlayRandomSound.h"

#include "Kismet/GameplayStatics.h"
#include "Soulslike.h"

void USLAN_PlayRandomSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (Sound) {
		UGameplayStatics::PlaySoundAtLocation(MeshComp, Sound, MeshComp->GetSocketLocation(socket_for_sound));
	}
	else {
		SLDEBUG("not sound for SLAN play sound");
	}
}