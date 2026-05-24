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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
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
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, MaxGroggy);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, Groggy);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, MaxLevel);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, Level);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, Damage);
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxHealth, Category="Attribute")
	FGameplayAttributeData MaxHealth;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Health, Category="Attribute")
	FGameplayAttributeData Health;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxStamina, Category="Attribute")
	FGameplayAttributeData MaxStamina;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Stamina, Category="Attribute")
	FGameplayAttributeData Stamina;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxMana, Category="Attribute")
	FGameplayAttributeData MaxMana;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Mana, Category="Attribute")
	FGameplayAttributeData Mana;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxPoise, Category="Attribute")
	FGameplayAttributeData MaxPoise;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Poise, Category="Attribute")
	FGameplayAttributeData Poise;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxPower, Category="Attribute")
	FGameplayAttributeData MaxPower;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Power, Category="Attribute")
	FGameplayAttributeData Power;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxGroggy, Category="Attribute")
	FGameplayAttributeData MaxGroggy;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Groggy, Category="Attribute")
	FGameplayAttributeData Groggy;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxLevel, Category="Attribute")
	FGameplayAttributeData MaxLevel;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Level, Category="Attribute")
	FGameplayAttributeData Level;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Damage, Category="Attribute")
	FGameplayAttributeData Damage;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxPoise(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Poise(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxPower(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Power(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxGroggy(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Groggy(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxLevel(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Damage(const FGameplayAttributeData& OldValue);

private:
	/** Timer used to clear State.Stamina.Spending after the player stops spending stamina. */
	FTimerHandle StaminaSpendingClearTimer;
};
