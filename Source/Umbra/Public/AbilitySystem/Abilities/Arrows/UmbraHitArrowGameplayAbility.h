// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraCDDurationGameplayAbility.h"
#include "UmbraHitArrowGameplayAbility.generated.h"

class AUmbraPlayerController;
/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraHitArrowGameplayAbility : public UUmbraCDDurationGameplayAbility
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
	
	void SpawnArrow(AUmbraPlayerController* PC, const FVector SpawnLocation, FRotator SpawnRotation, FVector Direction) const;



protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AUmbraArrowProjectile> ArrowClass;

private:
	FTimerHandle SpawnArrowTimerHandle;

	UPROPERTY(EditAnywhere)
	float DurationAnimMontage = 1.25f;

	UPROPERTY(EditAnywhere)
	float HeightOffset = 40.f;

	UPROPERTY(EditAnywhere)
	float ForwardOffset = 100.f;
	
	AActor* OriginalPawn = nullptr;
	AUmbraPlayerController* CachedPlayerController = nullptr;

	UFUNCTION()
	void OnArrowDestroyed(AActor* DestroyedActor);
};
