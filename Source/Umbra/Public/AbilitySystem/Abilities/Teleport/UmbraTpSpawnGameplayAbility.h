// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraCooldownGameplayAbility.h"
#include "UmbraTpSpawnGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraTpSpawnGameplayAbility : public UUmbraCooldownGameplayAbility
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport")
	float MaxTeleportDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport")
	float WallOffset = 20.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Portal")
	TSubclassOf<AActor> PortalClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
};
