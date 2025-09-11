// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "UmbraAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
