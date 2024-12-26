// Copyrighted by Vorona Games


#include "AbilitySystem/ModMagnitudeCalculation/MMC_Damage.h"

#include "UmbraGameplayTags.h"
#include "AbilitySystem/UmbraAttributeSet.h"

UMMC_Damage::UMMC_Damage()
{
	IncomingDamageDef.AttributeToCapture = UUmbraAttributeSet::GetIncomingDamageAttribute();
	IncomingDamageDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	IncomingDamageDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(IncomingDamageDef);
}

float UMMC_Damage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float DamageValue = Spec.GetSetByCallerMagnitude(FUmbraGameplayTags::Get().Combat_Damage, true, -1.f);
	return DamageValue;
}
