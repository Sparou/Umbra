// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"
#include "InteractionAbility.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UInteractionAbility : public UUmbraBaseGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:

	TWeakObjectPtr<AActor> CurrentInteractionTarget;

	UFUNCTION()
	void PrintString(float number);
};
