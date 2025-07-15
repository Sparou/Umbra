// Copyrighted by Vorona Games


#include "AbilitySystem/Interaction/InteractionAbility.h"

#include "AbilitySystemComponent.h"
#include "UmbraGameplayTags.h"
#include "AbilitySystem/Interaction/Tasks/AbilityTask_GrantNearbyInteraction.h"
#include "AbilitySystem/Interaction/Tasks/AbilityTask_UpdateInteractionTarget.h"
#include "AbilitySystem/Interaction/InteractionStatics.h"

void UInteractionAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilityTask_GrantNearbyInteraction* GrantNearbyInteraction = UAbilityTask_GrantNearbyInteraction::GrantNearbyInteraction(this, 500, 0.1f);
	GrantNearbyInteraction->ReadyForActivation();

	FInteractionQuery InteractionQuery;
	InteractionQuery.RequestingActor = GetAvatarActorFromActorInfo();
	InteractionQuery.RequestingController = Cast<AController>(GetAvatarActorFromActorInfo()->GetOwner());
	
	UAbilityTask_UpdateInteractionTarget* UpdateInteractionTarget = UAbilityTask_UpdateInteractionTarget::UpdateInteractionTarget(this, InteractionQuery, 200, 0.1, GetAvatarActorFromActorInfo()->GetActorLocation());
	UpdateInteractionTarget->ReadyForActivation();

	UpdateInteractionTarget->InteractableObjectsChanged.AddDynamic(this, &ThisClass::OnInteractableObjectChanged);
}

void UInteractionAbility::OnInteractableObjectChanged(const TArray<FInteractionOption>& InteractableOptions)
{
	UE_LOG(UmbraAbilitiesLog, Log, TEXT("Interaction options was updated in [%s]"), *GetNameSafe(this));
	this->InteractionOptions = InteractableOptions;
}

void UInteractionAbility::TriggerInteraction()
{
	if (InteractionOptions.IsEmpty())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		UE_LOG(UmbraAbilitiesLog, Display, TEXT("Trigger Interaction"));
		const FInteractionOption& InteractionOption = InteractionOptions[0];

		AActor* Instigator = GetAvatarActorFromActorInfo();
		AActor* InteractableActor = UInteractionStatics::GetActorFromInteractableInterface(InteractionOption.InteractionInterface);
		
		FGameplayEventData Payload;
		Payload.EventTag = FUmbraGameplayTags::Get().Ability_Interact;
		Payload.Instigator = Instigator;
		Payload.Target = InteractableActor;

		InteractionOption.InteractionInterface->CustomizeInteractionEventData(FUmbraGameplayTags::Get().Ability_Interact, Payload);

		AActor* TargetActor = const_cast<AActor*>(ToRawPtr(Payload.Target));
		
		FGameplayAbilityActorInfo ActorInfo;
		ActorInfo.InitFromActor(InteractableActor, TargetActor, InteractionOption.TargetAbilitySystemComponent);

		UE_LOG(UmbraAbilitiesLog, Display, TEXT("Interaction event send to [%s]"), *GetNameSafe(TargetActor));
		UE_LOG(UmbraAbilitiesLog, Display, TEXT("Data:"));
		UE_LOG(UmbraAbilitiesLog, Display, TEXT("ASC = [%s]"), *GetNameSafe(InteractionOption.TargetAbilitySystemComponent));

		const bool bSuccess = InteractionOption.TargetAbilitySystemComponent->TriggerAbilityFromGameplayEvent(
			InteractionOption.TargetInteractionAbilitySpecHandle,
			&ActorInfo,
			FUmbraGameplayTags::Get().Ability_Interact,
			&Payload,
			*InteractionOption.TargetAbilitySystemComponent
		);
	}
}
