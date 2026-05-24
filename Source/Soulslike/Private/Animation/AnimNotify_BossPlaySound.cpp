#include "Animation/AnimNotify_BossPlaySound.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_BossPlaySound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !Sound)
	{
		return;
	}

	if (bFollow)
	{
		UGameplayStatics::SpawnSoundAttached(
			Sound,
			MeshComp,
			AttachSocketName,
			FVector::ZeroVector,
			EAttachLocation::KeepRelativeOffset,
			true,
			VolumeMultiplier,
			PitchMultiplier);
		return;
	}

	const FVector SpawnLocation = MeshComp->GetSocketLocation(AttachSocketName);
	UGameplayStatics::PlaySoundAtLocation(
		MeshComp,
		Sound,
		SpawnLocation,
		VolumeMultiplier,
		PitchMultiplier);
}

FString UAnimNotify_BossPlaySound::GetNotifyName_Implementation() const
{
	return FString(TEXT("Boss Play Sound"));
}

