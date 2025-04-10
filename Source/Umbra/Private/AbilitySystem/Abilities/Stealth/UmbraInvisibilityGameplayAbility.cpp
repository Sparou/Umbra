// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Stealth/UmbraInvisibilityGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Character/UmbraBaseCharacter.h"
#include "Components/SkeletalMeshComponent.h"



void UUmbraInvisibilityGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (AActor* AvatarActor = ActorInfo->AvatarActor.Get())
	{
		if (AUmbraBaseCharacter* Character = Cast<AUmbraBaseCharacter>(AvatarActor))
		{
			Character->SetInvisibility(true);

			if (ActorInfo->AbilitySystemComponent.IsValid())
			{				
				ActorInfo->AbilitySystemComponent->AddLooseGameplayTag(InvisibleTag);
			}

		}
	}
}

void UUmbraInvisibilityGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
												   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
												   bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	// Показать персонажа (на сервере)
	if (AActor* AvatarActor = ActorInfo->AvatarActor.Get())
	{
		if (AUmbraBaseCharacter* Character = Cast<AUmbraBaseCharacter>(AvatarActor))
		{
			Character->SetInvisibility(false);
			
			if (ActorInfo->AbilitySystemComponent.IsValid())
			{
				ActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(InvisibleTag);
			}
		}
	}
}