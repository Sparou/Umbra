// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Aggressiveness.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UMMC_Aggressiveness : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
