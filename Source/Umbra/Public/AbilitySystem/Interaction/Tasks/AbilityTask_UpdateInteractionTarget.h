// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_UpdateInteractionTarget.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UAbilityTask_UpdateInteractionTarget : public UAbilityTask
{
	GENERATED_BODY()

	UAbilityTask_UpdateInteractionTarget(const FObjectInitializer& ObjectInitializer);

public:

	UFUNCTION(BlueprintCallable)

	static UAbilityTask_UpdateInteractionTarget* UpdateInteractionTarget(UGameplayAbility* OwningAbility, float InteractionScanRange, float InteractionScanRate, FVector StartLocation);

	virtual void Activate() override;

private:
		
	float InteractionScanRange = 100.f;
	float InteractionScanRate = 0.1f;
	FVector StartLocation = FVector::ZeroVector;

private:

	FTimerHandle TraceTimerHandle;
	
	virtual void OnDestroy(bool bInOwnerFinished) override;
};
