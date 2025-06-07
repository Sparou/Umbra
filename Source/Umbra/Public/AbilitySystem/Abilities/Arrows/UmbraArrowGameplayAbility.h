// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraCDDurationGameplayAbility.h"
#include "UmbraArrowGameplayAbility.generated.h"

class AUmbraPlayerController;

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraArrowGameplayAbility : public UUmbraCDDurationGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
								 const FGameplayAbilityActorInfo* ActorInfo,
								 const FGameplayAbilityActivationInfo ActivationInfo,
								 const FGameplayEventData* TriggerEventData) override;
	
	
	void SpawnArrow(AUmbraPlayerController* PC, FVector SpawnLocation, FRotator SpawnRotation, FVector Direction) const;


protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AUmbraArrowProjectile> ArrowClass;

	AActor* OriginalPawn = nullptr;

private:
	FTimerHandle SpawnArrowTimerHandle;

	UPROPERTY(EditAnywhere)
	float DurationAnimMontage = 0.5f;

	UPROPERTY(EditAnywhere)
	float HeightOffset = 40.f;

	UPROPERTY(EditAnywhere)
	float ForwardOffset = 100.f;
};
