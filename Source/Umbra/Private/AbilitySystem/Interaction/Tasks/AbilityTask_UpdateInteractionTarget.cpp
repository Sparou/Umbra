// Copyrighted by Vorona Games


#include "AbilitySystem/Interaction/Tasks/AbilityTask_UpdateInteractionTarget.h"

UAbilityTask_UpdateInteractionTarget::UAbilityTask_UpdateInteractionTarget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

UAbilityTask_UpdateInteractionTarget* UAbilityTask_UpdateInteractionTarget::UpdateInteractionTarget(
	UGameplayAbility* OwningAbility, float InteractionScanRange, float InteractionScanRate, FVector StartLocation)
{
	UAbilityTask_UpdateInteractionTarget* MyObj = NewAbilityTask<UAbilityTask_UpdateInteractionTarget>(OwningAbility);
	MyObj->InteractionScanRange = InteractionScanRange;
	MyObj->InteractionScanRate = InteractionScanRate;
	MyObj->StartLocation = StartLocation;
	return MyObj;
}

void UAbilityTask_UpdateInteractionTarget::Activate()
{
	SetWaitingOnAvatar();
	UWorld* World = GetWorld();
	//World->GetTimerManager().SetTimer(TraceTimerHandle, this, &ThisClass::)
}

void UAbilityTask_UpdateInteractionTarget::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}


