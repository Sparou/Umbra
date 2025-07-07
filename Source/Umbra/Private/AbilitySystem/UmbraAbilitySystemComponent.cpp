// Copyrighted by Vorona Games


#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"

void UUmbraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	for (const TSubclassOf<UGameplayAbility> Ability : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1);
		if (UUmbraBaseGameplayAbility* UmbraBaseAbility = Cast<UUmbraBaseGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(UmbraBaseAbility->StartupInputTag);
			GiveAbility(AbilitySpec);
		}
	}
}

void UUmbraAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);
	if (!Spec.IsActive()) return;
	
	const UGameplayAbility* Instance = Spec.GetPrimaryInstance();
	FPredictionKey OriginalPredictionKey = Instance ? Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : Spec.ActivationInfo.GetActivationPredictionKey();
	InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, OriginalPredictionKey);
}

void UUmbraAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);
	if (!Spec.IsActive()) return;

	const UGameplayAbility* Instance = Spec.GetPrimaryInstance();
	FPredictionKey OriginalPredictionKey = Instance ? Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : Spec.ActivationInfo.GetActivationPredictionKey();
	InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, OriginalPredictionKey);
}

void UUmbraAbilitySystemComponent::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			if (!AbilitySpec.IsActive()) TryActivateAbility(AbilitySpec.Handle);
			AbilitySpecInputPressed(AbilitySpec);
		}
	}
}

void UUmbraAbilitySystemComponent::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}
