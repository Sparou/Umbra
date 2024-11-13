// Copyrighted by Vorona Games


#include "Input/UmbraInputConfig.h"

const FUmbraInputAction* UUmbraInputConfig::GetInputActionByTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FUmbraInputAction& InputAction : AbilitiesInputActions)
	{
		if (InputAction.InputTag.MatchesTagExact(InputTag))
		{
			return &InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Input action with tag [%s] not found in [%s]"),
			*InputTag.ToString(),
			*GetNameSafe(this));
	}

	return nullptr;
}
