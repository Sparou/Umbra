// Copyrighted by Vorona Games


#include "AbilitySystem/UmbraAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/UmbraActiveGameplayAbility.h"

void UUmbraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	for (const TSubclassOf<UGameplayAbility> Ability : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1);
		if (UUmbraBaseGameplayAbility* UmbraBaseAbility = Cast<UUmbraBaseGameplayAbility>(AbilitySpec.Ability))
		{
			if (UUmbraActiveGameplayAbility* UmbraActiveAbility = Cast<UUmbraActiveGameplayAbility>(UmbraBaseAbility))
			{
				AbilitySpec.DynamicAbilityTags.AddTag(UmbraActiveAbility->StartupInputTag);
			}
			GiveAbility(AbilitySpec);
		}
	}
}

void UUmbraAbilitySystemComponent::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive()) TryActivateAbility(AbilitySpec.Handle);
		}
	}
}

void UUmbraAbilitySystemComponent::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}