// Copyrighted by Vorona Games


#include "AbilitySystem/AI/ModMagnitudeCalculation/MMC_Fearfulness.h"

float UMMC_Fearfulness::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	return FMath::RandRange(0.f, 100.f);
}
