// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Fearfulness.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UMMC_Fearfulness : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
