// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Character/Data/StealthKillsData.h"
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
	
	void RotateCharacterToTarget(const FVector& TargetLocation, float RotationInRate);
	void UpdateRotation();

	UFUNCTION()
	void MoveToKillPosition();
	UFUNCTION()
	void StartStealthKill();

	void SendEventToTarget();

	UFUNCTION()
	void OnMoveCompleted(bool bTimedOut, bool bReachedDestination, FVector FinalTargetLocation);
	
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageBlendingOut();
	
	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageCancelled();
	
	void EnableMovement() const;

	FStealthKillMontages StealthKillMontages;
	TObjectPtr<AActor> TargetActor;
	AUmbraPlayerCharacter* AvatarCharacter;

	UTraversalComponent* TraversalComponent;

	FVector AdditionalVerticalOffset;
	float MaxSpeed;
	
	FRotator DesiredRotation;
	FTimerHandle RotationTimerHandle;
	TFunction<void()> TimerCallback;
	FVector CameraOffset;
	FVector CameraDesiredLocation;
};
