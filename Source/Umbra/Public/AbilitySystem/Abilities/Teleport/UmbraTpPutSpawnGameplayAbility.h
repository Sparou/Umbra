// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraCDDurationGameplayAbility.h"
#include "UmbraTpPutSpawnGameplayAbility.generated.h"

class AUmbraPortalActor;
/**
 * 
 */
UCLASS() 
class UMBRA_API UUmbraTpPutSpawnGameplayAbility : public UUmbraCDDurationGameplayAbility
{
	GENERATED_BODY()

	UUmbraTpPutSpawnGameplayAbility();
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:
	// Настройки
	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	TSubclassOf<AUmbraPortalActor> PortalClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

private:
	static FVector LocationPortalStart;
	static bool bStartPlaced;	
};
