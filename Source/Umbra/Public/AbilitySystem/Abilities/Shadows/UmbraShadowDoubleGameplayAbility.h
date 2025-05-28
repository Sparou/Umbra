// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraCDDurationGameplayAbility.h"
#include "UmbraShadowDoubleGameplayAbility.generated.h"

class AUmbraBaseCharacter;
/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraShadowDoubleGameplayAbility : public UUmbraCDDurationGameplayAbility
{
	GENERATED_BODY()


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow")
	TSubclassOf<ACharacter> ShadowCharacterClass;

	UPROPERTY(EditAnywhere)
	bool IsShadow = true;

	UPROPERTY(EditAnywhere)
	int32 NumClones = 5;

	UPROPERTY(EditAnywhere)
	float Radius = 300.0f;

	UPROPERTY(EditAnywhere)
	float TimeBlockCharac = 0.5f;

	FTimerHandle MovementEnableTimer;
	
	UPROPERTY()
	AUmbraBaseCharacter* ShadowClone;
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
