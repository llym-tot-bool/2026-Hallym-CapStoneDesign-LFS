// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "SLCharacterAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API USLCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	USLCharacterAttributeSet();
	
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, MaxHealth);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, Health);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, MaxStamina);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, Stamina);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, MaxMana);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, Mana);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, MaxPoise);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, Poise);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, MaxPower);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, Power);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, MaxLevel);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, Level);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, Damage);
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
protected:
	UPROPERTY(BlueprintReadOnly, Category="Attribute")
	FGameplayAttributeData MaxHealth;
	UPROPERTY(BlueprintReadOnly, Category="Attribute")
	FGameplayAttributeData Health;
	
	UPROPERTY(BlueprintReadOnly, Category="Attribute")
	FGameplayAttributeData MaxStamina;
	UPROPERTY(BlueprintReadOnly, Category="Attribute")
	FGameplayAttributeData Stamina;
	
	UPROPERTY(BlueprintReadOnly, Category="Attribute")
	FGameplayAttributeData MaxMana;
	UPROPERTY(BlueprintReadOnly, Category="Attribute")
	FGameplayAttributeData Mana;
	
	UPROPERTY(BlueprintReadOnly, Category="Attribute")
	FGameplayAttributeData MaxPoise;
	UPROPERTY(BlueprintReadOnly, Category="Attribute")
	FGameplayAttributeData Poise;
	
	UPROPERTY(BlueprintReadOnly, Category="Attribute")
	FGameplayAttributeData MaxPower;
	UPROPERTY(BlueprintReadOnly, Category="Attribute")
	FGameplayAttributeData Power;
	
	UPROPERTY(BlueprintReadOnly, Category="Attribute")
	FGameplayAttributeData MaxLevel;
	UPROPERTY(BlueprintReadOnly, Category="Attribute")
	FGameplayAttributeData Level;
	
	UPROPERTY(BlueprintReadOnly, Category="Attribute")
	FGameplayAttributeData Damage;

private:
	/** Timer used to clear State.Stamina.Spending after the player stops spending stamina. */
	FTimerHandle StaminaSpendingClearTimer;
};
