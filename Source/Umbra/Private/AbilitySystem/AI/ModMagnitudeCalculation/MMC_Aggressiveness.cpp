// Copyrighted by Vorona Games


#include "AbilitySystem/AI/ModMagnitudeCalculation/MMC_Aggressiveness.h"

float UMMC_Aggressiveness::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	int val = FMath::RandRange(0.f, 100.f);
	return val;
}
