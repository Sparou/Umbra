// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_VisualAnalysis.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UMMC_VisualAnalysis : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
