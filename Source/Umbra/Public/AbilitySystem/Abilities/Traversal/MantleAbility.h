// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UmbraTraversalAbility.h"
#include "MantleAbility.generated.h"

struct FTraversalActionMontage;

/**
 * 
 */
UCLASS()
class UMBRA_API UMantleAbility : public UUmbraTraversalAbility
{
	GENERATED_BODY()

protected:

	/** Obstacle Detection */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Obstacle")
	float ObstacleDetectionDownOffset = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Obstacle")
	int32 ObstacleDetectionIterations = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Obstacle")
	float ObstacleDetectionDistance = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Obstacle")
	float ObstacleDetectionOffsetStep = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Obstacle")
	bool bObstacleDetectionDebug = false;
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
	bool bEdgeDetectionDebug = false;
	UPROPERTY(BlueprintReadOnly, Category = "Hits")
	FHitResult EdgeHitResult = FHitResult();

	/**Top Detection */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Top")
	int32 TopDetectionIterations = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Top")
	float TopDetectionTraceHeight = 90.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Top")
	float TopDetectionOffsetStep = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Top")
	bool bTopDetectionDebug = false;
	UPROPERTY(BlueprintReadOnly, Category = "Hits")
	FHitResult TopHitResult = FHitResult();

	/** Space Validation */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Space")
	float SpaceValidationXOffset = -30.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Space")
	float SpaceValidationZOffset = -30.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Space")
	bool bSpaceValidationDebug = false;

protected:
	
	UFUNCTION(BlueprintCallable)
	bool ValidateMantle();
	
	UFUNCTION(BlueprintCallable)
	bool FindTraversalActionMontage();
};
