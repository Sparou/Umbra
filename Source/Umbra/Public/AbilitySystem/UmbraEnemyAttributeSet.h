// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/UmbraAttributeSet.h"
#include "UmbraEnemyAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraEnemyAttributeSet : public UUmbraAttributeSet
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	/* Attributes */

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Fearfulness, Category = "Temperamental Attributes")
	FGameplayAttributeData Fearfulness;
	ATTRIBUTE_ACCESSORS(UUmbraEnemyAttributeSet, Fearfulness)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Aggressiveness, Category = "Temperamental Attributes")
	FGameplayAttributeData Aggressiveness;
	ATTRIBUTE_ACCESSORS(UUmbraEnemyAttributeSet, Aggressiveness)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_VisualAnalysis, Category = "Temperamental Attributes")
	FGameplayAttributeData VisualAnalysis;
	ATTRIBUTE_ACCESSORS(UUmbraEnemyAttributeSet, VisualAnalysis)

	/* Replication Functions */

	UFUNCTION()
	void OnRep_Fearfulness(const FGameplayAttributeData& OldFearfulness) const { GAMEPLAYATTRIBUTE_REPNOTIFY(UUmbraEnemyAttributeSet, Fearfulness, OldFearfulness); }

	UFUNCTION()
	void OnRep_Aggressiveness(const FGameplayAttributeData& OldAggressiveness) const { GAMEPLAYATTRIBUTE_REPNOTIFY(UUmbraEnemyAttributeSet, Aggressiveness, OldAggressiveness); }

	UFUNCTION()
	void OnRep_VisualAnalysis(const FGameplayAttributeData& OldVisualAnalysis) const { GAMEPLAYATTRIBUTE_REPNOTIFY(UUmbraEnemyAttributeSet, VisualAnalysis, OldVisualAnalysis); }
};
