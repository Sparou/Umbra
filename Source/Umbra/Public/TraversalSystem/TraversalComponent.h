// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UmbraGameplayTags.h"
#include "Components/ActorComponent.h"
#include "TraversalComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(TraversalComponent, Log, All);

class UCapsuleComponent;
class UCameraComponent;
class UCharacterMovementComponent;
class UMotionWarpingComponent;

USTRUCT(BlueprintType)
struct FTraversalActionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UAnimMontage* Montage = nullptr;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag InState = FUmbraGameplayTags::Get().Traversal_State_FreeRoam;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag OutState = FUmbraGameplayTags::Get().Traversal_State_FreeRoam;

	/*
	 * Offsets
	 * Нужны для того, чтобы изменять положение корневой
	 * кости пероснажа относительно WallTop с помощью
	 * Motion Warping. Помогает персонажу принять
	 * правильное положение относительно стены.
	 * 
	 */
	
	UPROPERTY(EditAnywhere)
	float Warp1XOffset = 0.f;

	UPROPERTY(EditAnywhere)
	float Warp1ZOffset = 0.f;
	
	UPROPERTY(EditAnywhere)
	float Warp2XOffset = 0.f;

	UPROPERTY(EditAnywhere)
	float Warp2ZOffset = 0.f;

	UPROPERTY(EditAnywhere)
	float Warp3XOffset = 0.f;

	UPROPERTY(EditAnywhere)
	float Warp3ZOffset = 0.f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMBRA_API UTraversalComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTraversalComponent();
	
private:
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
	FHitResult NextClimbResult = FHitResult();

	float WallHeight;
	float WallDepth;
	float VaultHeight;
	
	bool bInLand = true;

	FTraversalActionData CurrentActionData;
	
	/** Refernces */
	TObjectPtr<ACharacter> OwnerCharacter;
	TObjectPtr<UCharacterMovementComponent> CharacterMovement;
	TObjectPtr<UMotionWarpingComponent> MotionWarping;
	TObjectPtr<UCameraComponent> Camera;
	TObjectPtr<UCapsuleComponent> Capsule;
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;
	TObjectPtr<UAnimInstance> AnimInstance;
	
	/** Input Variables */
	float ForwardValue;
	float RightValue;

	FUmbraGameplayTags UGT = FUmbraGameplayTags::Get();
	
public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void TriggerTraversalAction(bool JumpAction);
	void GridScan(int GridWidth, int GridHeight, const FVector& ScanBaseLocation, const FRotator& ScanRotation);
	void PlayTraversalMontage();
	void AddMovementInput(float ScaleValue, bool Front);
	void ResetMovement();

	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Data")
	TMap<FGameplayTag, FTraversalActionData> TraversalActionDataMap;

	/* Wall Detection */
	UPROPERTY(EditDefaultsOnly, Category="Traversal|WallDetection")
	int32 DetectWallIterationsFalling = 8;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|WallDetection")
	int32 DetectWallIterationsNotFalling = 15;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|WallDetection")
	float DetectWallDownOffset = 60.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|WallDetection")
	float DetectWallUpStepMultiplier = 16.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|WallDetection")
	float DetectWallBackwardOffset = 20.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|WallDetection")
	float DetectWallForwardOffset = 140.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|WallDetection")
	float DetectWallSphereRadius = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|WallDetection|Debug")
	bool bDrawDetectWallDebug = false;


	
	/* Grid Scan */
    UPROPERTY(EditDefaultsOnly, Category="Traversal|GridScan")
    float GridColumnSpacing = 20.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|GridScan")
    float GridColumnOffsetMultiplier = 10.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|GridScan")
    float GridRowSpacing = 8.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|GridScan")
    float GridScanLineTraceDistance = 60.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|GridScan")
    float WallDifferenceThreshold = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|GridScan|Debug")
	bool bDrawWallEdgeDebug = false;


	
    /* Wall Top Search */
    UPROPERTY(EditDefaultsOnly, Category="Traversal|WallTopSearch")
    int32 TopScanIterations = 8;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|WallTopSearch")
    float TopForwardScanStep = 30.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|WallTopSearch")
    float TopElevatedOffset = 7.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|WallTopSearch")
    float TopSweepSphereRadius = 2.5f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|WallTopSearch|Debug")
	bool bDrawWallTopDebug = false;

	
	
    /* FindWallDepth */
    UPROPERTY(EditDefaultsOnly, Category="Traversal|FindWallDepth")
    float DepthForwardOffset = 30.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|FindWallDepth")
    float DepthSweepSphereRadius = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|FindWallDepth|Debug")
	bool bDrawWallDepthDebug = false;


	
    /* FindWallVault */
    UPROPERTY(EditDefaultsOnly, Category="Traversal|FindWallVault")
    float VaultForwardOffset = 70.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|FindWallVault")
    float VaultDownOffset = 200.f;

    UPROPERTY(EditDefaultsOnly, Category="Traversal|FindWallVault")
    float VaultSweepSphereRadius = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|FindWallVault|Debug")
	bool bDrawWallVaultDebug = false;

	bool FindWallEdge(int GridWidth, int GridHeight, const FVector& ScanBaseLocation, const FRotator& ScanRotation, FHitResult& OutWallEdgeResult);
	bool FindWallTop(const FHitResult& WallEdgeHit, const FRotator& WallRot, FHitResult& OutWallTopResult, FHitResult& OutLastTopHit, bool& OutEndOfWallFound);
	bool FindWallDepth(const FHitResult& LastTopHit, const FRotator& WallRot, FHitResult& OutWallDepthResult);
	bool FindWallVault(const FHitResult& WallDepthHit, const FRotator& WallRot, FHitResult& OutWallVaultResult);


	
	/* DecideClimbStyle */
	UPROPERTY(EditDefaultsOnly, Category="Traversal|DecideClimbStyle")
	float DecideClimbStyleDownOffset = 125.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Traversal|DecideClimbStyle")
	float DecideClimbStyleBackwardOffset = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|DecideClimbStyle")
	float DecideClimbStyleForwardOffset = 25.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|DecideClimbStyle")
	float DecideClimbStyleSphereRadius = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Traversal|DecideClimbStyle|Debug")
	bool bDecideClimbStyleEnableDebug = false;


	
	/* Climb Movement */

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|WallDetection")
	FName HandBoneName = "hand_l";
	
	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|WallDetection")
	int32 ClimbMovementWallDetectionIterations = 3;

	/* Определяет, насколько выше относительно корневой кости будет совершаться поиск стены для карабканья */
	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|WallDetection")
	float ClimbMovementWallDetectionBracedHeightOffset = 200.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|WallDetection")
	float ClimbMovementWallDetectionFreeHangHeightOffset = 220.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|WallDetection")
	float ClimbMovementWallDetectionHorizontalOffset = 10.f;

	/* Определяет вертикальное смещение каждой итерации поиска стены для карабканья */
	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|WallDetection")
	float ClimbMovementWallDetectionVerticalMultiplier = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|WallDetection")
	float ClimbMovementWallDetectionDistance = 60.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|TopDetection")
	int32 ClimbMovementTopDetectionIterations = 7;
	
	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|TopDetection")
	float ClimbMovementTopDetectionInitialForwardOffset = 2.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|TopDetection")
	float ClimbMovementTopDetectionInitialVerticalOffset = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|TopDetection")
	float ClimbMovementTopDetectionVerticalOffsetMultiplier = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|TopDetection")
	float ClimbMovementTopDetectionDistance = 5.f;

	/* Определяет смещение персонажа по оси X относительно точки ClimbTopResult */
	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement")
	float ClimbMovementBracedXOffset = 45.f;

	/* Определяет смещение персонажа по оси X относительно точки ClimbTopResult */
	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement")
	float ClimbMovementFreeHangXOffset = 17.f;

	/* Определяет смещение персонажа по оси Z относительно точки ClimbTopResult */
	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement")
	float ClimbMovementBracedZOffset = 107.f;

	/* Определяет смещение персонажа по оси Z относительно точки ClimbTopResult */
	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement")
	float ClimbMovementFreeHangZOffset = 130.f;


	
	/* ClimbCheckForSides  */
	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|CheckForSides")
	int32 ClimbCheckForSidesIterations = 6;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|CheckForSides")
	float ClimbCheckForSidesInitialVerticalOffset = 2.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|CheckForSides")
	float ClimbCheckForSidesVerticalOffsetMultiplier = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|CheckForSides")
	float ClimbCheckForSidesHorizontalOffset = 15.f;


	
	/* ValidateClimbMovementSurface */

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|SurfaceValidation")
	float ClimbMovementSurfaceValidationHorizontalOffset = 13.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|SurfaceValidation")
	float ClimbMovementSurfaceValidationInitialVerticalOffset = 90.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|SurfaceValidation")
	float ClimbMovementSurfaceValidationCapsuleStartOffset = 40.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|SurfaceValidation")
	float ClimbMovementSurfaceValidationCapsuleEndOffset = 25.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|SurfaceValidation")
	float ClimbMovementSurfaceValidationCapsuleRadius = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|SurfaceValidation")
	float ClimbMovementSurfaceValidationCapsuleHalfHeight = 82.f;


	
	/* IK */
	
	/* Устанавливает максимальное расстояние от NextClimbTopResult (WallTopResult) при поиске позиции для IK рук */
	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float HandIKMaxDistance = 16.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float HandIKTraceDistance = 20.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float HandIKWallDetectionSphereRadius = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	FRotator HandIKLeftHandAdditiveRotation = FRotator(90.f, 0, 280.f);

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	FRotator HandIKRightHandAdditiveRotation = FRotator(270.f, 0, 270.f);

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	int32 HandIKTopDetectionIterations = 7;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float HandIKTopDetectionForwardOffset = 2.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float HandIKTopDetectionVerticalOffsetMultiplier = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float HandIKTopDetectionTraceDistance = 75.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float HandIKTopDetectionSphereRadius = 2.5f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float HandIKLocationVerticalOffset = 9.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	int32 FootIKWallDetectionIterations = 2;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float FootIKWallDetectionVerticalOffsetMultiplier = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float FootIKLeftHorizontalOffset = 9.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float FootIKRightHorizontalOffset = 9.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float FootIKLeftVerticalOffset = 140.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float FootIKRightVerticalOffset = 140.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float FootIKTraceBackwardOffset = 30.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float FootIKTraceForwardOffset = 30.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float FootIKWallDetectionSphereRadius = 6.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Initialization")
	float FootIKLocationAdditiveBackwardOffset = 17.f;

	//////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Update")
	int32 HandIKUpdateWallDetectionIterations = 5;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Update")
	float HandIKUpdateClimbHandSpace = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Update")
	float HandIKUpdateWallDetectionHorizontalOffsetMultiplier = 2.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Update")
	float HandIKUpdateWallDetectionTraceStartOffset = 50.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Update")
	float HandIKUpdateWallDetectionTraceDistance = 70.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Update")
	float HandIKUpdateWallDetectionSphereRadius = 15.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Update")
	int32 HandIKUpdateTopDetectionIterations = 6;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Update")
	float HandIKUpdateTopDetectionForwardOffset = 2.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Update")
	float HandIKUpdateTopDetectionVerticalOffsetMultiplier = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Update")
	float  HandIKUpdateTopDetectionTraceDistance = 75.f;

	UPROPERTY(EditDefaultsOnly, Category="Traversal|ClimbMovement|IK|Update")
	float  HandIKUpdateTopDetectionSphereRadius = 2.5f;

	int32 FootIKUpdateWallDetectionIterations = 3;

	float FootIKUpdateVerticalOffsetMultiplier = 5.f;

	float FootIKUpdateLeftVerticalOffset = 135.f;

	float FootIKUpdateRightVerticalOffset = 125.f;

	float FootIKUpdateLeftHorizontalOffset = 4.f;

	float FootIKUpdateRightHorizontalOffset = 4.f;

	float FootIKUpdateTraceBackwardOffset = 30.f;

	float FootIKUpdateTraceForwardOffset = 70.f;

	float FootIKUpdateWallDetectionSphereRadius = 6.f;
	

private:
	
    void InitializeReferences();

	/* Setters */
	void SetTraversalStateSettings(ECollisionEnabled::Type CollisionType, EMovementMode MovementMode, const bool& StopMovementImmediately);
	void SetTraversalState(const FGameplayTag& NewTraversalState);
	void SetClimbStyle(const FGameplayTag& NewClimbStyle);
	void SetClimbDirection (const FGameplayTag& NewClimbDirection);
	void SetTraversalAction(const FGameplayTag& NewTraversalAction);

	/* Helper Functions */
	float ClimbValues (const FGameplayTag& NewClimbStyle, const float& Braced, const float& FreeHang) const;
	FVector VectorDirectionMove (const FVector& Source, const FGameplayTag& Direction, const float& MoveValue);
	FVector VectorDirectionMoveWithRotation (const FVector& Source, const FGameplayTag& Direction, const float& MoveValue, const FRotator& Rotation);
	FRotator ReverseNormal (const FVector& Normal);

	FHitResult DetectWall();
	
	FHitResult FindWallEdge(int GridWidth, int GridHeight, const FVector& ScanBaseLocation, const FRotator& ScanRotation);
	void MeasureWall();
	void DecideTraversalType(bool JumpAction);
	void ResetTraversalResults();
	FVector FindWarpLocation(const FVector& Location, const FRotator& Rotation, float XOffset, float ZOffset);
	void DecideClimbStyle(const FVector& Location, const FRotator& Rotation);

	UFUNCTION(BlueprintCallable)
	void NextClimbHandIK(const bool bLeftHand);
	UFUNCTION(BlueprintCallable)
	void NextClimbFootIK(const bool bLeftFoot);
	UFUNCTION(BlueprintCallable)
	void UpdateHandIK(const bool bLeftHand);
	UFUNCTION(BlueprintCallable)
	void UpdateFootIK(const bool bLeftFoot);

	void ResetFootIK();
	
	void ClimbMovementIK();
	
	/** Validate Functions */
	void ValidateIsInLand();
	bool ValidateClimbMovementSurface(const FVector& MovementImpactLocation);

	/** Input Functions */
	void ClimbMovement();
	void StopClimbMovement();
	bool ClimbCheckForSides(const FVector& ImpactPoint);
	void SetNewClimbPosition(float NewLocationX, float NewLocationY, float NewLocationZ, FRotator NewRotation);


};
