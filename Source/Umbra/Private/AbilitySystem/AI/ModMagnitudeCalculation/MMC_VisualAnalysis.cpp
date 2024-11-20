// Copyrighted by Vorona Games


#include "AbilitySystem/AI/ModMagnitudeCalculation/MMC_VisualAnalysis.h"

float UMMC_VisualAnalysis::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	return FMath::RandRange(0.f, 100.f);
}
