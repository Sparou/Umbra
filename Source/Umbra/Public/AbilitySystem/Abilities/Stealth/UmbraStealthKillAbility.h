// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"
#include "Character/UmbraPlayerCharacter.h"
#include "UmbraStealthKillAbility.generated.h"

UENUM()
enum EStealthKillPosition
{
	Behind,
	Front,
	Top,
	Ledge,
	Left,
	Right
};

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraStealthKillAbility : public UUmbraBaseGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> StealthKillEffect;
	
private:

	UFUNCTION()
	void OnMoveCompleted();
	
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageCancelled();
	
	void EnableMovement() const;

	EStealthKillPosition CheckAvatarActorPosition() const;

	FStealthKillMontages StealthKillMontages;
	TObjectPtr<AActor> TargetActor;
};
