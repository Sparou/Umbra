// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraCDDurationGameplayAbility.h"
#include "UmbraLineShadowGameplayAbility.generated.h"

class AUmbraLineShadowController;
class AUmbraBaseCharacter;
/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraLineShadowGameplayAbility : public UUmbraCDDurationGameplayAbility
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow")
	TSubclassOf<ACharacter> LineShadowCharacterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow|Clones")
	TArray<AUmbraLineShadowController*> LineShadowControllers;
	
	UPROPERTY(EditAnywhere)
	bool IsShadow = true;

	UPROPERTY(EditAnywhere)
	int32 NumClones = 4;

	UPROPERTY(EditAnywhere)
	float Spacing = 200.f;

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

