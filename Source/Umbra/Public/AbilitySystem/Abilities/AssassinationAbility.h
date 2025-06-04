// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"
#include "Character/Data/AssassinationsData.h"
#include "AssassinationAbility.generated.h"

class AUmbraPlayerCharacter;
/**
 * 
 */
UCLASS()
class UMBRA_API UAssassinationAbility : public UUmbraBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	float HeightDifferenceThreshold;

	UPROPERTY(EditDefaultsOnly)
	float ActivationDistance = 170.f;

	UPROPERTY(EditDefaultsOnly)
	float RandomSeedMultiplier = 100.f;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	AUmbraPlayerCharacter* SourceCharacter;
	AActor* TargetCharacter;
	FAssassinationData AssassinationData;
	
	void StartAssassination();
	void SendEventToTarget();
	
	UFUNCTION()
	void OnInitiatorMontageCompleted();
	UFUNCTION()
	void OnInitiatorMontageBlendOut();

	bool InitializeSourceCharacter();
	bool InitializeTargetCharacter();
	bool InitializeAssassinationData();
	bool ValidateActivationDistance();
	
	void EnableMovement();
};
