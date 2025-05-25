// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Traps/UmbraPutTrapGameplayAbility.h"

#include "Actors/Traps/UmbraTrap.h"
#include "Character/UmbraPlayerCharacter.h"

void UUmbraPutTrapGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	AActor* AvatarActor = ActorInfo->AvatarActor.Get();

	if (AUmbraTrap* Trap = GetWorld()->SpawnActor<AUmbraTrap>(TrapClass, AvatarActor->GetActorLocation(), FRotator::ZeroRotator))
	{
		Trap->InitializedCharacter = Cast<AUmbraPlayerCharacter>(AvatarActor);
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}