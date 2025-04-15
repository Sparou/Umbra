// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraCooldownGameplayAbility.h"
#include "UmbraTelepHitGameplayAbility.generated.h"

/**
 * 
 */


UCLASS()
class UMBRA_API UUmbraTelepHitGameplayAbility : public UUmbraCooldownGameplayAbility
{
	GENERATED_BODY()

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	void TeleportBackAndEnd(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport|Hit")
	float DurationTelep = 0.5f;
	
	FTimerHandle TeleportBackTimerHandle;
	FVector InitialLocation;
	
	// Настройки
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport")
	float MaxTeleportDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport")
	float WallOffset = 20.0f;
	
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	
};
