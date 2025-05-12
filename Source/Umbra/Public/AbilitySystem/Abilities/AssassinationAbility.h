// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"
#include "Character/Data/StealthKillsData.h"
#include "AssassinationAbility.generated.h"

struct FStealthKillMontages;
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
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	
	AUmbraPlayerCharacter* Initiator;
	AActor* Target;
	FStealthKillMontages AssassinationMontages;

	UPROPERTY(ReplicatedUsing = OnRep_RandomSeed)
	int32 RandomSeed = -1;

	UFUNCTION()
	void OnRep_RandomSeed();
	
	void StartAssassination();
	void SendEventToTarget();
	UFUNCTION()
	void OnInitiatorMontageCompleted();
	UFUNCTION()
	void OnInitiatorMontageBlendOut();

	void EnableMovement();
};
