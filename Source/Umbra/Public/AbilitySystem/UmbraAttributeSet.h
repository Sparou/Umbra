// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "UmbraAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties() {}

	FGameplayEffectContextHandle EffectContextHandle;
	
	/* Source (effect causer) properties */
	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	/* Target (this) properties */
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;
	
	UPROPERTY()
	AController* TargetController = nullptr;
	
	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
	
	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;
};

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/* Attributes */
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UUmbraAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UUmbraAttributeSet, MaxHealth)

	/* Replication Functions */
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const { GAMEPLAYATTRIBUTE_REPNOTIFY(UUmbraAttributeSet, Health, OldHealth); }
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const { GAMEPLAYATTRIBUTE_REPNOTIFY(UUmbraAttributeSet, MaxHealth, OldMaxHealth); }

private:

	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props);
};
