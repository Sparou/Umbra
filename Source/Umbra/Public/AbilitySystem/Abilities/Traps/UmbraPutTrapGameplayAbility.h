// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"
#include "AbilitySystem/Abilities/UmbraCooldownGameplayAbility.h"
#include "UmbraPutTrapGameplayAbility.generated.h"

class AUmbraTrap;
/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraPutTrapGameplayAbility : public UUmbraCooldownGameplayAbility
{
	GENERATED_BODY()
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:
	// Настройки
	UPROPERTY(EditDefaultsOnly, Category = "Trap")
	TSubclassOf<AUmbraTrap> TrapClass;
};

