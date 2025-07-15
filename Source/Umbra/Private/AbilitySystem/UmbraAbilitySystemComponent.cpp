// Copyrighted by Vorona Games

#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"

DEFINE_LOG_CATEGORY(UmbraAbilitySystemLog)

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

void UUmbraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;

	UE_LOG(UmbraAbilitySystemLog, Log, TEXT("Number of Abilities = [%d]"), GetActivatableAbilities().Num());
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.Ability && (Spec.GetDynamicSpecSourceTags().HasTag(InputTag)))
		{
			UE_LOG(UmbraAbilitySystemLog, Log, TEXT("[%s] was proccesed"), *GetNameSafe(Spec.Ability));
			InputPressedHandles.AddUnique(Spec.Handle);
			InputHeldHandles.AddUnique(Spec.Handle);
		}
	}
}

void UUmbraAbilitySystemComponent::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;

	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability && (Spec.GetDynamicSpecSourceTags().HasTag(InputTag)))
		{
			InputReleasedHandles.AddUnique(Spec.Handle);
			InputHeldHandles.Remove(Spec.Handle);
		}
	}
}


void UUmbraAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePause)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();
  
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldHandles)
	{
		if (const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (Spec->Ability && !Spec->IsActive())
			{
				AbilitiesToActivate.AddUnique(Spec->Handle);
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedHandles)
	{
		if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (Spec->Ability)
			{
				Spec->InputPressed = true;

				if (Spec->IsActive())
				{
					AbilitySpecInputPressed(*Spec);
				}
				else
				{
					AbilitiesToActivate.AddUnique(Spec->Handle);
				}
			}
		}
	}


	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedHandles)
	{
		TryActivateAbility(SpecHandle);
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedHandles)
	{
		if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (Spec->Ability)
			{
				Spec->InputPressed = false;

				if (Spec->IsActive())
				{
					AbilitySpecInputReleased(*Spec);
				}
			}
		}
	}

	InputPressedHandles.Reset();
	InputReleasedHandles.Reset();
}
