// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/UmbraCooldownGameplayAbility.h"


void UUmbraCooldownGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
											   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
											   bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bWasCancelled)
	{
		Super::EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else if (!GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle))
	{
		// Запускаем таймер, по окончании которого вызовется FinishAbility
		GetWorld()->GetTimerManager().SetTimer(
			CooldownTimerHandle,
			FTimerDelegate::CreateUObject(this, &UUmbraCooldownGameplayAbility::FinishAbility),
			CooldownDuration, false);
	}
}

void UUmbraCooldownGameplayAbility::FinishAbility()
{
	Super::EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}