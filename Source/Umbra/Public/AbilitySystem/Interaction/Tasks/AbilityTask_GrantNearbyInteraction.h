// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_GrantNearbyInteraction.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UAbilityTask_GrantNearbyInteraction : public UAbilityTask
{
	GENERATED_BODY()

	UAbilityTask_GrantNearbyInteraction(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;

	UFUNCTION()
	static UAbilityTask_GrantNearbyInteraction* GrantNearbyInteraction(UGameplayAbility* OwningAbility, float InteractionScanRange, float InteractionScanRate);

private:

	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	void QueryInteractables();

	float InteractionScanRange = 100.f;
	float InteractionScanRate = 0.1f;

	FTimerHandle QueryTimerHandle;
};
