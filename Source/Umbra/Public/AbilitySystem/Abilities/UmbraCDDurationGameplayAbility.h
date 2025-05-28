// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraCooldownGameplayAbility.h"
#include "UmbraCDDurationGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraCDDurationGameplayAbility : public UUmbraCooldownGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Ability|Duration")
	float AbilityDuration = 5.0f;

protected:
	FTimerHandle EndAbilityTimerHandle;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
