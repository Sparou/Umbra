// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/UmbraCDDurationGameplayAbility.h"


void UUmbraCDDurationGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	// Если уже запущен таймер, не запускаем повторно
	if (!GetWorld()->GetTimerManager().IsTimerActive(EndAbilityTimerHandle))
	{
		// Запускаем таймер, по окончании которого вызовется 
		GetWorld()->GetTimerManager().SetTimer(
			EndAbilityTimerHandle,
			FTimerDelegate::CreateLambda([this, SpecHandle = CurrentSpecHandle, ActorInfo = CurrentActorInfo, ActivationInfo = CurrentActivationInfo]() {
				EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, false);
			}),
			AbilityDuration, false);
	}
}