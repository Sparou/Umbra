// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"
#include "UmbracooldownGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraCooldownGameplayAbility : public UUmbraBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Ability|Duration")
	float CooldownDuration = 5.0f;
	
protected:
	FTimerHandle CooldownTimerHandle;
	
	// Вызывается, когда способность должна завершиться после задержки
	virtual void FinishAbility();
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
