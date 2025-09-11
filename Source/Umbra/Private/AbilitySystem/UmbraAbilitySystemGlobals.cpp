// Copyrighted by Vorona Games


#include "AbilitySystem/UmbraAbilitySystemGlobals.h"

#include "AbilitySystem/GameplayEffect/UmbraGameplayEffectContext.h"

FGameplayEffectContext* UUmbraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FUmbraGameplayEffectContext();
}
