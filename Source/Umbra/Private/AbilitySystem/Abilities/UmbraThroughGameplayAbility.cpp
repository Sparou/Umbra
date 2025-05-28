// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/UmbraThroughGameplayAbility.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Components/CapsuleComponent.h"

void UUmbraThroughGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (AUmbraPlayerCharacter* AvatarCharacter = Cast<AUmbraPlayerCharacter>(ActorInfo->AvatarActor.Get()))
	{
		AvatarCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
	}
}

void UUmbraThroughGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if (AUmbraPlayerCharacter* AvatarCharacter = Cast<AUmbraPlayerCharacter>(GetAvatarActorFromActorInfo()))
	{
		UCapsuleComponent* CapsuleComponent = AvatarCharacter->GetCapsuleComponent();
		if (!CapsuleComponent) return;

		CapsuleComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	}
}