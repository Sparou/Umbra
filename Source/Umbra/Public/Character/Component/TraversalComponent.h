// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UmbraGameplayTags.h"
#include "Components/ActorComponent.h"
#include "TraversalComponent.generated.h"


class UCapsuleComponent;
class UCameraComponent;
class UCharacterMovementComponent;
class UMotionWarpingComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMBRA_API UTraversalComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTraversalComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void TriggerTraversalAction(bool JumpAction);
	void GridScan(int GridWidth, int GridHeight, const FVector& ScanBaseLocation, const FRotator& ScanRotation);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool FindWallEdge(int GridWidth, int GridHeight, const FVector& ScanBaseLocation, const FRotator& ScanRotation, FHitResult& OutWallEdgeResult);
	bool FindWallTop(const FHitResult& WallEdge, const FRotator& WallRot, FHitResult& OutWallTopResult, FHitResult& OutLastTopHit);
	bool FindWallDepth(const FHitResult& LastTopHit, const FRotator& WallRot, FHitResult& OutWallDepthResult);
	bool FindWallVault(const FHitResult& WallDepth, const FRotator& WallRot, FHitResult& OutWallVaultResult);

	/** Tracing Settings */

	// Для GridScan / FindWallEdge
    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float GridColumnSpacing = 20.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float GridColumnOffsetMultiplier = 10.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float GridRowSpacing = 8.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float TraceDistance = 60.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float WallDifferenceThreshold = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing|Debug")
	bool bDrawWallEdgeDebug = true;

    // Для FindWallTop
    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    int32 TopScanIterations = 8;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float TopForwardScanStep = 30.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float TopElevatedOffset = 7.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float TopSweepSphereRadius = 2.5f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing|Debug")
	bool bDrawWallTopDebug = true;

    // Для FindWallDepth
    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float DepthForwardOffset = 30.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float DepthSweepSphereRadius = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing|Debug")
	bool bDrawWallDepthDebug = true;

    // Для FindWallVault
    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float VaultForwardOffset = 70.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float VaultDownOffset = 200.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float VaultSweepSphereRadius = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing|Debug")
	bool bDrawWallVaultDebug = true;

    // Для DetectWall
    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    int32 DetectWallIterationsFalling = 8;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    int32 DetectWallIterationsNotFalling = 15;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float DetectWallDownOffset = 60.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float DetectWallUpStepMultiplier = 16.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float DetectWallBackwardOffset = 20.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float DetectWallForwardOffset = 140.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing")
    float DetectWallSphereRadius = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|Tracing|Debug")
	bool bDrawDetectWallDebug = true;

private:
	
    void InitializeReferences();
	void SetTraversalStateSettings(ECollisionEnabled::Type CollisionType, EMovementMode MovementMode, const bool& StopMovementImmediately);
	void SetTraversalState(const FGameplayTag& NewTraversalState);
	void SetClimbStyle(const FGameplayTag& NewClimbStyle);
	void SetClimbDirection (const FGameplayTag& NewClimbDirection);
	float ClimbValues (const FGameplayTag& NewClimbStyle, const float& Braced, const float& FreeHang) const;
	FVector VectorDirectionMove (const FVector& Source, const FGameplayTag& Direction, const float& MoveValue);
	FVector VectorDirectionMoveWithRotation (const FVector& Source, const FGameplayTag& Direction, const float& MoveValue, const FRotator& Rotation);
	FRotator ReverseNormal (const FVector& Normal);
	FHitResult DetectWall();
	FHitResult FindWallEdge(int GridWidth, int GridHeight, const FVector& ScanBaseLocation, const FRotator& ScanRotation);
	
	FGameplayTag TraversalState = FUmbraGameplayTags::Get().Traversal_State_FreeRoam;
	FGameplayTag ClimbStyle = FUmbraGameplayTags::Get().Traversal_ClimbStyle_BracedClimb;
	FGameplayTag ClimbDirection = FUmbraGameplayTags::Get().Traversal_Direction_NoDirection;
	FGameplayTag TraversalAction = FUmbraGameplayTags::Get().Traversal_Action_NoAction;

	/** Wall Variables */ 
	FRotator WallRotation = FRotator();
	FHitResult WallHitResult = FHitResult();
	FHitResult WallEdgeResult = FHitResult();
	FHitResult WallTopResult = FHitResult();
	FHitResult WallDepthResult = FHitResult();
	FHitResult WallVaultResult = FHitResult();
	
	
	/** Components */
	TObjectPtr<ACharacter> OwnerCharacter;
	TObjectPtr<UCharacterMovementComponent> CharacterMovement;
	TObjectPtr<UMotionWarpingComponent> MotionWarping;
	TObjectPtr<UCameraComponent> Camera;
	TObjectPtr<UCapsuleComponent> Capsule;
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;
	TObjectPtr<UAnimInstance> AnimInstance;

	FUmbraGameplayTags UGT = FUmbraGameplayTags::Get();
		
};
