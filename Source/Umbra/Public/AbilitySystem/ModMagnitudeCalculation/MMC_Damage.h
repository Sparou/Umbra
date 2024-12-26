// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Damage.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UMMC_Damage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:

	UMMC_Damage();
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition IncomingDamageDef;
};
