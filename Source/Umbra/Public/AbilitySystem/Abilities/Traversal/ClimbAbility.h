// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Traversal/UmbraTraversalAbility.h"
#include "ClimbAbility.generated.h"

class AUmbraPlayerController;
/**
 * 
 */
UCLASS()
class UMBRA_API UClimbAbility : public UUmbraTraversalAbility
{
	GENERATED_BODY()

protected:

	/** Obstacle Detection */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Obstacle")
	float ObstacleDetectionDownOffset = -20.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Obstacle")
	int32 ObstacleDetectionIterations = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Obstacle")
	float ObstacleDetectionDistance = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Obstacle")
	float ObstacleDetectionOffsetStep = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Obstacle")
	bool ObstacleDetectionDebug = false;
	UPROPERTY(BlueprintReadOnly, Category = "Hits")
	FHitResult ObstacleHitResult = FHitResult();
	
	/** Edge Detection */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Edge")
	int32 EdgeDetectionIterations = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Edge")
	float EdgeDetectionOffsetStep = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Edge")
	float EdgeDetectionDistance = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Edge")
	float EdgeDetectionThreshold = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Edge")
	bool EdgeDetectionDebug = false;
	UPROPERTY(BlueprintReadOnly, Category = "Hits")
	FHitResult EdgeHitResult = FHitResult();

	/**Top Detection */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Top")
	int32 TopDetectionIterations = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Top")
	float TopDetectionTraceHeight = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Top")
	float TopDetectionOffsetStep = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Top")
	bool TopDetectionDebug = false;
	UPROPERTY(BlueprintReadOnly, Category = "Hits")
	FHitResult TopHitResult = FHitResult();
	UPROPERTY(BlueprintReadOnly, Category = "Hits")
	FHitResult NextTopHitResult = FHitResult();

	/** Surface Validation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Surface")
	float SurfaceValidationXOffset = -30.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Surface")
	float SurfaceValidationZOffset = -30.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Surface")
	bool SurfaceValidationDebug = false;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ClimbEffectClass;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Accessors")
	AUmbraPlayerController* GetUmbraPlayerController();
	
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintCallable)
	virtual bool ValidateInitialClimbSurface();
	
	UFUNCTION(BlueprintCallable)
	bool FindTraversalActionMontage();

	UFUNCTION(BlueprintCallable)
	FActiveGameplayEffectHandle ApplyClimbEffectToOwner();

private:

	TWeakObjectPtr<AUmbraPlayerController> UmbraPlayerController;
	
};
