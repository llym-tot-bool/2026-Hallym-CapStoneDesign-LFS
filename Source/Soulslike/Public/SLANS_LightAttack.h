

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"

#include "SLANS_LightAttack.generated.h"

/**
 * 
 */
UCLASS(abstract)
class SOULSLIKE_API USLANS_LightAttack : public UAnimNotifyState
{
	GENERATED_BODY()

public:
    USLANS_LightAttack();
	~USLANS_LightAttack();

public:
    // The Tag to send to the GA (must match HitEventTag in GA)
    UPROPERTY(EditAnywhere, Category = "GAS")
    FGameplayTag EventTag;

    UPROPERTY(EditAnywhere, Category = "Collision")
    FVector BoxHalfExtents = FVector(50.f, 50.f, 50.f);

    UPROPERTY(EditAnywhere, Category = "Collision")
    FName SocketName = "Socket_weapon_base";

    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, 
        const FAnimNotifyEventReference& EventReference) override;
};
