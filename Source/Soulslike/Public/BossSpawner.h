#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossSpawner.generated.h"

class USceneComponent;
class ABoss;
class APawn;

UCLASS()
class SOULSLIKE_API ABossSpawner : public AActor
{
	GENERATED_BODY()

public:
	ABossSpawner();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	TSubclassOf<ABoss> BossClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Detection", meta = (ClampMin = "100.0"))
	float DetectionRange = 1200.0f;

	// 전방 감지 반각(도). 45이면 총 90도 콘 영역.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Detection", meta = (ClampMin = "1.0", ClampMax = "89.0"))
	float DetectionHalfAngleDeg = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Detection|Debug")
	bool bShowDetectionDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	bool bSpawnOnlyOnce = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	bool bDestroyAfterSpawn = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner")
	bool bHasSpawnedBoss = false;

	bool IsPawnDetectedInFront(const APawn* Pawn) const;
	void TryDetectAndSpawn();

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	ABoss* SpawnBoss();
};
