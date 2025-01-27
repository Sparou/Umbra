// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"
#include "Character/UmbraPlayerCharacter.h"
#include "UmbraStealthKillAbility.generated.h"

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

	UPROPERTY(EditDefaultsOnly)
	float HeightDifferenceThreshold = 100.f;
	
private:

	EStealthKillPosition CheckAvatarActorPosition() const;
	void RotateCharacterToTarget(const FVector& TargetLocation, float RotationInRate);
	void UpdateRotation();
	void MoveToKillPosition();
	void StartStealthKill();

	UFUNCTION()
	void OnMoveCompleted();
	
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageCancelled();
	
	void EnableMovement() const;

	FStealthKillMontages StealthKillMontages;
	TObjectPtr<AActor> TargetActor;
	AUmbraPlayerCharacter* AvatarCharacter;
	
	FRotator DesiredRotation;
	FTimerHandle RotationTimerHandle;
	TFunction<void()> TimerCallback;
	FVector CameraOffset;
	FVector CameraDesiredLocation;
};
