// Copyrighted by Vorona Games


#include "AbilitySystem/Interaction/InteractionAbility.h"
#include "AbilitySystem/Interaction/Tasks/AbilityTask_GrantNearbyInteraction.h"
#include "AbilitySystem/Interaction/Tasks/AbilityTask_UpdateInteractionTarget.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"

void UInteractionAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilityTask_GrantNearbyInteraction* GrantNearbyInteraction = UAbilityTask_GrantNearbyInteraction::GrantNearbyInteraction(this, 500, 0.1f);
	GrantNearbyInteraction->ReadyForActivation();
	UAbilityTask_UpdateInteractionTarget* UpdateInteractionTarget = UAbilityTask_UpdateInteractionTarget::UpdateInteractionTarget(this, 200, 0.1, GetAvatarActorFromActorInfo()->GetActorLocation());
	UpdateInteractionTarget->ReadyForActivation();

	UpdateInteractionTarget->UpdateInteractionTargetDelegate.AddLambda([this](AActor* NewInteractionTarget)
	{
		UE_LOG(UmbraAbilitiesLog, Log, TEXT("Interaction Target [%s] was updated in [%s]"), *GetNameSafe(NewInteractionTarget), *GetNameSafe(this));
		CurrentInteractionTarget = NewInteractionTarget;
	});
	
	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this, /*bTestAlreadyPressed=*/false);

	// Привязываем функцию-обработчик
	WaitInputPress->OnPress.AddDynamic(this, &UInteractionAbility::PrintString);
	WaitInputPress->ReadyForActivation();
}

void UInteractionAbility::PrintString(float number)
{
	UE_LOG(UmbraAbilitiesLog, Log, TEXT("Pressed"));
}
