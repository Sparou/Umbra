// Copyrighted by Vorona Games


#include "Input/UmbraInputConfig.h"

const UInputAction* UUmbraInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FUmbraInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Native input action with tag [%s] not found in [%s]"),
			*InputTag.ToString(),
			*GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UUmbraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FUmbraInputAction& Action : AbilitiesInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Ability input action with tag [%s] not found in [%s]"),
			*InputTag.ToString(),
			*GetNameSafe(this));
	}

	return nullptr;
}
