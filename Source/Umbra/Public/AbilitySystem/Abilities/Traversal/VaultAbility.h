// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UmbraTraversalAbility.h"
#include "VaultAbility.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UVaultAbility : public UUmbraTraversalAbility
{
	GENERATED_BODY()

protected:
	
	/** Obstacle Detection */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Obstacle")
	float ObstacleDetectionDownOffset = 40.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Obstacle")
	int32 ObstacleDetectionIterations = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Obstacle")
	float ObstacleDetectionDistance = 200.f;
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
	float EdgeDetectionDistance = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Edge")
	float EdgeDetectionThreshold = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Edge")
	bool bEdgeDetectionDebug = false;
	UPROPERTY(BlueprintReadOnly, Category = "Hits")
	FHitResult EdgeHitResult = FHitResult();

	/** Top Detection */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Top")
	int32 TopDetectionIterations = 5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Top")
	float TopDetectionTraceHeight = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Top")
	float TopDetectionOffsetStep = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Top")
	bool bTopDetectionDebug = false;
	UPROPERTY(BlueprintReadOnly, Category = "Hits")
	FHitResult TopHitResult = FHitResult();

	/** Depth Detection */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Depth")
	int32 DepthDetectionIterations = 20;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Depth")
	float DepthDetectionOffsetStep = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Depth")
	float DepthDetectionThreshold = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Depth")
	bool bDepthDetectionDebug = false;
	UPROPERTY(BlueprintReadOnly, Category = "Hits")
	FHitResult DepthHitResult = FHitResult();

	/** Vault Detection */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Vault")
	float VaultDetectionDistance = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Vault")
	float VaultDetectionMaxHeight = 500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Vault")
	bool bVaultDetectionDebug = false;
	UPROPERTY(BlueprintReadOnly, Category = "Hits")
	FHitResult VaultHitResult = FHitResult();

	/** Space Validation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Space")
	float SpaceValidationZOffset = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection|Space")
	bool bSpaceValidationDebug = false;

protected:
	
	UFUNCTION(BlueprintCallable)
	bool ValidateVault();
	
	UFUNCTION(BlueprintCallable)
	bool FindTraversalActionMontage();
	
};
