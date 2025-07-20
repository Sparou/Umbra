// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AttributeSet/UmbraAttributeSet.h"
#include "StealthAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UStealthAttributeSet : public UUmbraAttributeSet
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Visibility, Category = "Stealth Attributes")
	FGameplayAttributeData Visibility;
	ATTRIBUTE_ACCESSORS(UStealthAttributeSet, Visibility)

protected:

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	
	UFUNCTION()
	void OnRep_Visibility(const FGameplayAttributeData& OldVisibility) const { GAMEPLAYATTRIBUTE_REPNOTIFY(UStealthAttributeSet, Visibility, OldVisibility); }
};
