// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraCDDurationGameplayAbility.h"
#include "UmbraDomeSoundGameplayAbility.generated.h"

class AUmbraSoundDome;
/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraDomeSoundGameplayAbility : public UUmbraCDDurationGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
								 const FGameplayAbilityActorInfo* ActorInfo,
								 const FGameplayAbilityActivationInfo ActivationInfo,
								 const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
							const FGameplayAbilityActorInfo* ActorInfo,
							const FGameplayAbilityActivationInfo ActivationInfo,
							bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Sound")
	TSubclassOf<AUmbraSoundDome> DomeActorClass;
	

	UPROPERTY(EditAnywhere, Category = "Sound")
	float DomeRadius = 800.f;
};
