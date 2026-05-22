// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CardDataAsset.generated.h"

UENUM(BlueprintType)
enum class ECardEmblem : uint8
{
	Eye,
	Axe,
	Sword,
	Claw,
	Dice,
	Hammer
};

UENUM(BlueprintType)
enum class ERarity : uint8
{
	Normal,
	Rare,
	Epic
};

UCLASS(BlueprintType)
class SOULSLIKE_API UCardDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity", meta = (MultiLine = true))
	FText CardDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	ECardEmblem CardType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	ERarity Rarity;

	// Use TSubclassOf here so it exposes the Blueprint class types of your effects
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	TSubclassOf<class UGameplayEffect> EffectToApply;
};