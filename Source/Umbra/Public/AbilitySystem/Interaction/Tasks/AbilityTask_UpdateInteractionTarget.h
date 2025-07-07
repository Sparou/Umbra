// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Interface/InteractionInterface.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_UpdateInteractionTarget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FUpdateInteractionTargetDelegate, AActor*);

/**
 * 
 */
UCLASS()
class UMBRA_API UAbilityTask_UpdateInteractionTarget : public UAbilityTask
{
	GENERATED_BODY()

	UAbilityTask_UpdateInteractionTarget(const FObjectInitializer& ObjectInitializer);

public:

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_UpdateInteractionTarget* UpdateInteractionTarget(UGameplayAbility* OwningAbility, float InteractionScanRange, float InteractionScanRate, FVector StartLocation);

	virtual void Activate() override;
	FUpdateInteractionTargetDelegate UpdateInteractionTargetDelegate;

private:
		
	float InteractionScanRange = 100.f;
	float InteractionScanRate = 0.1f;
	FVector StartLocation = FVector::ZeroVector;

private:

	FTimerHandle TraceTimerHandle;
	
	virtual void OnDestroy(bool bInOwnerFinished) override;

	void PerformTrace();
	void LineTrace(FHitResult& OutHitResult, const UWorld* World, const FVector& Start, const FVector& End, ECollisionChannel Channel, FCollisionQueryParams Params);
	void AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, float MaxRange, FVector& OutTraceEnd, bool bIgnoredPitch) const;
	static bool ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition);

	TWeakObjectPtr<AActor> CurrentInteractionTarget;
	
	void HandleSuccessfulHit(const FHitResult& HitResult);
	void HandleUnsuccessfulHit(const FHitResult& HitResult);
};
