// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UmbraTraversalAbility.h"
#include "ClimbMoveAbility.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UClimbMoveAbility : public UUmbraTraversalAbility
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere)
	FHitResult TopHitResult = FHitResult();
	FHitResult NextTopHitResult = FHitResult();
	
	/** Movement */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MovementSurfaceValidationXOffset = -30.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MovementSurfaceValidationZOffset = -30.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MovementSurfaceValidationOffset = 300.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MovementSurfaceValidationStep = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MovementLedgeValidationDistance = 300.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MovementLedgeValidationTraceHeight = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	bool bMovementDebug = false;

	/** Animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations|Braced")
	TObjectPtr<UAnimMontage> BracedClimbLeftMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations|Braced")
	TObjectPtr<UAnimMontage> BracedClimbRightMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations|FreeHang")
	TObjectPtr<UAnimMontage> FreeHangClimbLeftMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations|FreeHang")
	TObjectPtr<UAnimMontage> FreeHangClimbRightMontage;

	UFUNCTION(BlueprintCallable)
	bool ValidateClimbMove(float Direction);
	bool ValidateClimbMoveSurface(float Direction);
	bool ValidateClimbMoveLedge(float Direction);

	UFUNCTION(BlueprintCallable)
	bool TryGetCurrentTopHitResult(FHitResult& OutHitResult);
};
