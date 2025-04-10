// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/UmbraCooldownGameplayAbility.h"


void UUmbraCooldownGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

void UUmbraCooldownGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
											   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
											   bool bReplicateEndAbility, bool bWasCancelled)
{
	
	// Если уже запущен таймер, не запускаем повторно
	if (!GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle))
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