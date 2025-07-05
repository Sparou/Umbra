// Copyrighted by Vorona Games


#include "AbilitySystem/Interaction/Tasks/AbilityTask_GrantNearbyInteraction.h"
#include "UmbraCollisionChannels.h"
#include "AbilitySystem/Interaction/InteractionStatics.h"

UAbilityTask_GrantNearbyInteraction::UAbilityTask_GrantNearbyInteraction(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

UAbilityTask_GrantNearbyInteraction* UAbilityTask_GrantNearbyInteraction::GrantNearbyInteraction(
	UGameplayAbility* OwningAbility,
	float InteractionScanRange,
	float InteractionScanRate)
{
	UAbilityTask_GrantNearbyInteraction* MyObj = NewAbilityTask<UAbilityTask_GrantNearbyInteraction>(OwningAbility);
	MyObj->InteractionScanRange = InteractionScanRange;
	MyObj->InteractionScanRate = InteractionScanRate;
	return MyObj;
}

void UAbilityTask_GrantNearbyInteraction::Activate()
{
	SetWaitingOnAvatar();
	GetWorld()->GetTimerManager().SetTimer(QueryTimerHandle, this, &ThisClass::QueryInteractables, InteractionScanRate, true);
}

void UAbilityTask_GrantNearbyInteraction::QueryInteractables()
{
	UWorld* World = GetWorld();
	AActor* ActorOwner = GetAvatarActor();

	if (World && ActorOwner)
	{
		FCollisionQueryParams Params(SCENE_QUERY_STAT(UAbilityTask_GrantNearbyInteraction), false);

		TArray<FOverlapResult> OverlapResults;
		World->OverlapMultiByChannel(
			OUT OverlapResults,
			ActorOwner->GetActorLocation(),
			FQuat::Identity,
			ECC_Interaction,
			FCollisionShape::MakeSphere(InteractionScanRange),
			Params);

		if (OverlapResults.Num() > 0)
		{
			TArray<TScriptInterface<IInteractionInterface>> InteractableActors;
			//UInteractionStatics::AppendInteractablesFromOverlapResult(OverlapResults, OUT InteractableActors);
		}
	}
}

void UAbilityTask_GrantNearbyInteraction::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}


