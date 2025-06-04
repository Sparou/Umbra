// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "TraversalActionsData.h"
#include "UmbraGameplayTags.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TraversalComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(TraversalComponentLog, Log, All);

struct FTraversalActionData;
class UCapsuleComponent;
class UCameraComponent;
class UCharacterMovementComponent;
class UMotionWarpingComponent;

UENUM()
enum class ETraversalDebugType : uint8
{
    None,
    ForOneFrame,
    ForDuration,
    Persistent
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMBRA_API UTraversalComponent : public UActorComponent
{
    GENERATED_BODY()

public:	
    UTraversalComponent();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category="Actions")
    TObjectPtr<UTraversalActionsData> TraversalActions;
    
    /*
     * Определяет максимальную высоту стены, при которой
     * пероснаж, стоящий на земле, может соврешить действие climb
     */
    float ClimbMaximumWallHeight = 250.f;
    
    /*
     * Определяет минимальную высоту стены, при которой
     * могут быть совершены действия Mantle и Vault
     */
    UPROPERTY(EditAnywhere, Category="Transitions")
    float MantleMinimumWallHeight = 45.f;

    
    /*
     * Определяет максимальную высоту стены, при которой
     * могут быть совершены действия Mantle и Vault
     */
    UPROPERTY(EditAnywhere, Category="Transitions")
    float MantleMaximumWallHeight = 160.f;

    /*
     * Определяет минимальную глубину стены, при которой
     * может быть совершено действие Vault.
     */
    UPROPERTY(EditAnywhere, Category="Transitions")
    float VaultMinimumDepth = 0.0f;

    /*
     * Определяет максимальную глубину стены, при которой
     * может быть совершено действие Vault
     */
    UPROPERTY(EditAnywhere, Category="Transitions")
    float VaultMaximumDepth = 120.f;
    
    /*
     * Определяет минимальную высоту Wall Depth относительно
     * Wall Vault, при которой может быть совершено действие vault
     */
    UPROPERTY(EditAnywhere, Category="Transitions")
    float VaultMinimumHeight = 60.f;


    /*
     * Определяет максимальную высоту Wall Depth относительно
     * Wall Vault, при которой может быть совершено действие vault
     */
    UPROPERTY(EditAnywhere, Category="Transitions")
    float VaultMaximumHeight = 120.f;
    
    /*
     * Определяет минимальную velocity персонажа, при
     * которой может быть совершенно действие vault
     */
    UPROPERTY(EditAnywhere, Category="Transitions")
    float VaultMinimumVelocity = 20.f;

    
    
    /* Wall Detection */
    
    UPROPERTY(EditDefaultsOnly, Category="Wall Detection")
    int32 DetectWallIterationsFalling = 8;
    
    UPROPERTY(EditDefaultsOnly, Category="Wall Detection")
    int32 DetectWallIterationsNotFalling = 15;
    
    UPROPERTY(EditDefaultsOnly, Category="Wall Detection")
    float DetectWallDownOffset = 60.f;
    
    UPROPERTY(EditDefaultsOnly, Category="Wall Detection")
    float DetectWallUpStepMultiplier = 16.f;
    
    UPROPERTY(EditDefaultsOnly, Category="Wall Detection")
    float DetectWallBackwardOffset = 20.f;

    UPROPERTY(EditDefaultsOnly, Category="Wall Detection")
    float DetectWallForwardOffset = 140.f;

    UPROPERTY(EditDefaultsOnly, Category="Wall Detection")
    float DetectWallSphereRadius = 10.f;

    UPROPERTY(EditDefaultsOnly, Category="Wall Detection|Debug")
    ETraversalDebugType DetectWallDebugType = ETraversalDebugType::None;

    UPROPERTY(EditDefaultsOnly, Category="Wall Detection|Debug")
    FColor DetectWallDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Wall Detection|Debug")
    FColor DetectWallDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Wall Detection|Debug")
    float DetectWallDebugTime = 3.f;


    
    /* Wall Edge Search */
    
    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Edge Search")
    float GridColumnSpacing = 20.f;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Edge Search")
    float GridColumnOffsetMultiplier = 10.f;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Edge Search")
    float GridRowSpacing = 8.f;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Edge Search")
    float GridScanLineTraceDistance = 60.f;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Edge Search")
    float WallDifferenceThreshold = 5.f;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Edge Search|Debug")
    ETraversalDebugType WallEdgeSearchDebugType = ETraversalDebugType::None;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Edge Search|Debug")
    FColor WallEdgeSearchDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Edge Search|Debug")
    FColor WallEdgeSearchDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Edge Search|Debug")
    float WallEdgeSearchDebugTime = 3.f;


    
    /* Wall Top Search */
    
    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Top Search")
    int32 WallTopSearchScanIterations = 8;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Top Search")
    float WallTopSearchForwardScanStep = 30.f;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Top Search")
    float WallTopSearchElevatedOffset = 7.f;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Top Search")
    float WallTopSearchSphereRadius = 2.5f;
    
    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Top Search|Debug")
    ETraversalDebugType WallTopSearchDebugType = ETraversalDebugType::None;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Top Search|Debug")
    FColor WallTopSearchDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Top Search|Debug")
    FColor WallTopSearchDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Top Search|Debug")
    float WallTopSearchDebugTime = 3.f;
    

    
    /* Wall Depth Search  */
    
    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Depth Search")
    float WallDepthSearchForwardOffset = 30.f;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Depth Search")
    float WallDepthSearchSphereRadius = 10.f;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Depth Search|Debug")
    ETraversalDebugType WallDepthSearchDebugType = ETraversalDebugType::None;
    
    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Depth Search|Debug")
    FColor WallDepthSearchDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Depth Search|Debug")
    FColor WallDepthSearchDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Depth Search|Debug")
    float WallDepthSearchDebugTime = 3.f;

    
    /* Wall Vault Search */
    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Vault Search")
    float WallVaultSearchForwardOffset = 70.f;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Vault Search")
    float WallVaultSearchDownOffset = 200.f;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Vault Search")
    float WallVaultSearchSphereRadius = 10.f;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Vault Search|Debug")
    ETraversalDebugType WallVaultSearchDebugType = ETraversalDebugType::None;
    
    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Vault Search|Debug")
    FColor WallVaultSearchDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Vault Search|Debug")
    FColor WallVaultSearchDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Grid Scan|Wall Vault Search|Debug")
    float WallVaultSearchDebugTime = 3.f;
    
    
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

    UPROPERTY(EditDefaultsOnly, Category="Climbing")
    float ClimbSpeed = 55.f;

    /* Определяет смещение персонажа по оси X относительно точки ClimbTopResult */
    UPROPERTY(EditDefaultsOnly, Category="Climbing")
    float ClimbMovementBracedXOffset = 45.f;

    /* Определяет смещение персонажа по оси Z относительно точки ClimbTopResult */
    UPROPERTY(EditDefaultsOnly, Category="Climbing")
    float ClimbMovementBracedZOffset = 95.f;

    /* Определяет смещение персонажа по оси X относительно точки ClimbTopResult */
    UPROPERTY(EditDefaultsOnly, Category="Climbing")
    float ClimbMovementFreeHangXOffset = 25.f;

    /* Определяет смещение персонажа по оси Z относительно точки ClimbTopResult */
    UPROPERTY(EditDefaultsOnly, Category="Climbing")
    float ClimbMovementFreeHangZOffset = 110.f;

    // Так как Dedicated Server может не обновлять позиции костей, для него
    // задаются конкретные значения для снижения нагрузки.
    //  Вычислеяется относительно ActorLocation.Z 
    UPROPERTY(EditDefaultsOnly, Category="Climbing")
    float ClimbMovementDedicatedServerBracedHandZOffset = 75.f;

    // Так как Dedicated Server может не обновлять позиции костей, для него
    // задаются конкретные значения для снижения нагрузки. 
    // Вычислеяется относительно ActorLocation.Z 
    UPROPERTY(EditDefaultsOnly, Category="Climbing")
    float ClimbMovementDedicatedServerFreeHangHandZOffset = 95.f;
    
    UPROPERTY(EditDefaultsOnly, Category="Climbing")
    float IsDroppingResetTime = 0.5f;


    UPROPERTY(EditDefaultsOnly, Category="Climbing|Surface Validation")
    float ValidateClimbSurfaceAdditiveBackwardOffset = 10.f;
    
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Surface Validation|Debug")
    ETraversalDebugType ClimbSurfaceValidationDebugType = ETraversalDebugType::None;
    
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Surface Validation|Debug")
    FColor ClimbSurfaceValidationDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Surface Validation|Debug")
    FColor ClimbSurfaceValidationDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Surface Validation|Debug")
    float ClimbSurfaceValidationDebugTime = 3.f;


    
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Wall Detection")
    int32 ClimbMovementWallDetectionIterations = 3;

    /* Определяет, насколько выше относительно корневой кости будет совершаться поиск стены для карабканья */
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Wall Detection")
    float ClimbMovementWallDetectionBracedHeightOffset = 180.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Wall Detection")
    float ClimbMovementWallDetectionFreeHangHeightOffset = 200.f;
    
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Wall Detection")
    float ClimbMovementWallDetectionHorizontalOffset = 10.f;

    /* Определяет вертикальное смещение каждой итерации поиска стены для карабканья */
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Wall Detection")
    float ClimbMovementWallDetectionVerticalMultiplier = 10.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Wall Detection")
    float ClimbMovementWallDetectionDistance = 60.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Wall Detection")
    float ClimbMovementWallDetectionSphereRadius = 5.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Wall Detection|Debug")
    ETraversalDebugType ClimbMovementWallDetectionDebugType = ETraversalDebugType::None;
    
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Wall Detection|Debug")
    FColor ClimbMovementWallDetectionDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Wall Detection|Debug")
    FColor ClimbMovementWallDetectionDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Wall Detection|Debug")
    float ClimbMovementWallDetectionDebugTime = 3.f;

    

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Top Detection")
    int32 ClimbMovementTopDetectionIterations = 7;
    
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Top Detection")
    float ClimbMovementTopDetectionInitialForwardOffset = 2.f;
    
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Top Detection")
    float ClimbMovementTopDetectionInitialVerticalOffset = 5.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Top Detection")
    float ClimbMovementTopDetectionVerticalOffsetMultiplier = 5.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Top Detection")
    float ClimbMovementTopDetectionDistance = 5.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Top Detection")
    float ClimbMovementTopDetectionSphereRadius = 2.5f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Top Detection|Debug")
    ETraversalDebugType ClimbMovementTopDetectionDebugType = ETraversalDebugType::None;
    
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Top Detection|Debug")
    FColor ClimbMovementTopDetectionDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Wall Detection|Debug")
    FColor ClimbMovementTopDetectionDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Top Detection|Debug")
    float ClimbMovementTopDetectionDebugTime = 3.f;

    
    /* ClimbCheckForSides  */
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Check For Sides")
    int32 ClimbCheckForSidesIterations = 6;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Check For Sides")
    float ClimbCheckForSidesInitialVerticalOffset = 2.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Check For Sides")
    float ClimbCheckForSidesVerticalOffsetMultiplier = 5.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Check For Sides")
    float ClimbCheckForSidesHorizontalOffset = 15.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Check For Sides|Debug")
    ETraversalDebugType ClimbCheckForSidesDebugType = ETraversalDebugType::None;
    
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Check For Sides|Debug")
    FColor ClimbCheckForSidesDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Check For Sides|Debug")
    FColor ClimbCheckForSidesDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Check For Sides|Debug")
    float ClimbCheckForSidesDebugTime = 3.f;

    
    /* Movement Surface Validation */

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Movement Surface Validation")
    float ClimbMovementSurfaceValidationHorizontalOffset = 13.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Movement Surface Validation")
    float ClimbMovementSurfaceValidationInitialVerticalOffset = 90.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Movement Surface Validation")
    float ClimbMovementSurfaceValidationCapsuleStartOffset = 40.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Movement Surface Validation")
    float ClimbMovementSurfaceValidationCapsuleEndOffset = 25.f;
    
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Movement Surface Validation")
    float ClimbMovementSurfaceValidationCapsuleRadius = 5.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Movement Surface Validation")
    float ClimbMovementSurfaceValidationCapsuleHalfHeight = 82.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Movement Surface Validation|Debug")
    ETraversalDebugType ClimbMovementSurfaceValidationDebugType = ETraversalDebugType::None;
    
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Movement Surface Validation|Debug")
    FColor ClimbMovementSurfaceValidationDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Climbing|Movement Surface Validation|Debug")
    FColor ClimbMovementSurfaceValidationHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Movement Surface Validation|Debug")
    float ClimbMovementSurfaceValidationDebugTime = 3.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Air Hang")	
    float AirHangMinDistance = 70.f;

    UPROPERTY(EditDefaultsOnly, Category="Climbing|Air Hang")	
    float AirHangMaxHeightDifference = 30.f;
    
    
    /* Inverse Kinematics */
    
    /* Устанавливает максимальное расстояние от NextClimbTopResult (WallTopResult) при поиске позиции для IK рук */
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization")
    float HandIKMaxDistance = 16.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Wall Detection")
    float HandIKTraceDistance = 20.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Wall Detection")
    float HandIKWallDetectionSphereRadius = 5.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Wall Detection|Debug")
    ETraversalDebugType HandIKWallDetectionDebugType = ETraversalDebugType::None;
    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Wall Detection|Debug")
    FColor HandIKWallDetectionDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Wall Detection|Debug")
    FColor HandIKWallDetectionDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Wall Detection|Debug")
    float HandIKWallDetectionDebugTime = 3.f;
    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization")
    FRotator HandIKLeftHandAdditiveRotation = FRotator(90.f, 0, 280.f);

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization")
    FRotator HandIKRightHandAdditiveRotation = FRotator(270.f, 0, 270.f);
    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Top Detection")
    int32 HandIKTopDetectionIterations = 7;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Top Detection")
    float HandIKTopDetectionForwardOffset = 2.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Top Detection")
    float HandIKTopDetectionVerticalOffsetMultiplier = 5.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Top Detection")
    float HandIKTopDetectionTraceDistance = 75.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Top Detection")
    float HandIKTopDetectionSphereRadius = 2.5f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization")
    float HandIKLocationVerticalOffset = 9.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Top Detection|Debug")
    ETraversalDebugType HandIKTopDetectionDebugType = ETraversalDebugType::None;
    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Top Detection|Debug")
    FColor HandIKTopDetectionDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Top Detection|Debug")
    FColor HandIKTopDetectionDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Top Detection|Debug")
    float HandIKTopDetectionDebugTime = 3.f;


    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Wall Detection")
    int32 FootIKWallDetectionIterations = 2;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Wall Detection")
    float FootIKWallDetectionVerticalOffsetMultiplier = 5.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization")
    float FootIKLeftHorizontalOffset = 9.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization")
    float FootIKRightHorizontalOffset = 9.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization")
    float FootIKLeftVerticalOffset = 140.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization")
    float FootIKRightVerticalOffset = 140.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Wall Detection")
    float FootIKTraceBackwardOffset = 30.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Wall Detection")
    float FootIKTraceForwardOffset = 30.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Wall Detection")
    float FootIKWallDetectionSphereRadius = 6.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization")
    float FootIKLocationAdditiveBackwardOffset = 17.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Wall Detection|Debug")
    ETraversalDebugType FootIKDebugType = ETraversalDebugType::None;
    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Wall Detection|Debug")
    FColor FootIKDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Wall Detection|Debug")
    FColor FootIKDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Initialization|Wall Detection|Debug")
    float FootIKDebugTime = 3.f;
    

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation")
    float HandIKUpdateClimbHandSpace = 10.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection")
    int32 HandIKUpdateWallDetectionIterations = 5;
    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection")
    float HandIKUpdateWallDetectionHorizontalOffsetMultiplier = 2.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection")
    float HandIKUpdateWallDetectionTraceStartOffset = 50.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection")
    float HandIKUpdateWallDetectionTraceDistance = 70.f;
    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection")
    float HandIKUpdateWallDetectionSphereRadius = 15.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection|Debug")
    ETraversalDebugType HandIKUpdateWallDetectionDebugType = ETraversalDebugType::None;
    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection|Debug")
    FColor HandIKUpdateWallDetectionDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection|Debug")
    FColor HandIKUpdateWallDetectionDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection|Debug")
    float HandIKUpdateWallDetectionDebugTime = 3.f;


    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Top Detection")
    int32 HandIKUpdateTopDetectionIterations = 6;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Top Detection")
    float HandIKUpdateTopDetectionForwardOffset = 2.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Top Detection")
    float HandIKUpdateTopDetectionVerticalOffsetMultiplier = 5.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Top Detection")
    float  HandIKUpdateTopDetectionTraceDistance = 75.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Top Detection")
    float  HandIKUpdateTopDetectionSphereRadius = 2.5f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Top Detection|Debug")
    ETraversalDebugType HandIKUpdateTopDetectionDebugType = ETraversalDebugType::None;
    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Top Detection|Debug")
    FColor HandIKUpdateTopDetectionDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Top Detection|Debug")
    FColor HandIKUpdateTopDetectionDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Top Detection|Debug")
    float HandIKUpdateTopDetectionDebugTime = 3.f;

    
    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection")
    int32 FootIKUpdateWallDetectionIterations = 3;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection")
    float FootIKUpdateVerticalOffsetMultiplier = 5.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection")
    float FootIKUpdateLeftVerticalOffset = 135.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection")
    float FootIKUpdateRightVerticalOffset = 125.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection")
    float FootIKUpdateLeftHorizontalOffset = 4.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection")
    float FootIKUpdateRightHorizontalOffset = 4.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection")
    float FootIKUpdateTraceBackwardOffset = 30.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection")
    float FootIKUpdateTraceForwardOffset = 70.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection")
    float FootIKUpdateWallDetectionSphereRadius = 6.f;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection|Debug")
    ETraversalDebugType FootIKUpdateWallDetectionDebugType = ETraversalDebugType::None;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection|Debug")
    FColor FootIKUpdateWallDetectionDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection|Debug")
    FColor FootIKUpdateWallDetectionDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Inverse Kinematics|Updation|Wall Detection|Debug")
    float FootIKUpdateWallDetectionDebugTime = 3.f;



    UPROPERTY(EditDefaultsOnly, Category="Hop")
    int32 HopGridScanWidth = 5;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    int32 HopGridScanHeight = 20;
    
    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopVerticalDistanceMultiplier = 25.f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopHorizontalDistanceMultiplier = 100.f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopForwardDirectionVerticalDistance = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopForwardRightDirectionVerticalDistance = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopRightDirectionVerticalDistance = -2.5f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopBackwardRightDirectionVerticalDistance = -4.0f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopBackwardDirectionVerticalDistance = -7.5f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopBackwardLeftDirectionVerticalDistance = -4.0f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopLeftDirectionVerticalDistance = -2.5f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopForwardLeftDirectionVerticalDistance = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopForwardDirectionHorizontalDistance = 0.0f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopForwardRightDirectionHorizontalDistance = 0.75f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopRightDirectionHorizontalDistance = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopBackwardRightDirectionHorizontalDistance = 0.75f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopBackwardDirectionHorizontalDistance = 0.0f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopBackwardLeftHorizontalDistance = -0.75f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopLeftDirectionHorizontalDistance = -1.0f;

    UPROPERTY(EditDefaultsOnly, Category="Hop")
    float HopForwardLeftDirectionHorizontalDistance = -0.75f;


    
    UPROPERTY(EditDefaultsOnly, Category="Mantle")
    float MantleSurfaceValidationCapsuleVerticalOffset = 10.f;

    UPROPERTY(EditDefaultsOnly, Category="Mantle|Debug")
    ETraversalDebugType MantleSurfaceValidationDebugType = ETraversalDebugType::None;

    UPROPERTY(EditDefaultsOnly, Category="Mantle|Debug")
    FColor MantleSurfaceValidationDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Mantle|Debug")
    FColor MantleSurfaceValidationDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Mantle|Debug")
    float MantleSurfaceValidationDebugTime = 3.f;


    

    UPROPERTY(EditDefaultsOnly, Category="Vault")
    float VaultOnObstacleCapsuleTraceVerticalOffset = 20.f;

    UPROPERTY(EditDefaultsOnly, Category="Vault")
    float VaultCapsuleTraceVerticalOffset = 10.f;
    
    UPROPERTY(EditDefaultsOnly, Category="Vault|Debug")
    ETraversalDebugType VaultOnObstacleSurfaceValidationDebugType = ETraversalDebugType::None;

    UPROPERTY(EditDefaultsOnly, Category="Vault|Debug")
    FColor VaultOnObstacleSurfaceValidationDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Vault|Debug")
    FColor VaultOnObstacleSurfaceValidationDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Vault|Debug")
    float VaultOnObstacleSurfaceValidationDebugTime = 3.f;

    UPROPERTY(EditDefaultsOnly, Category="Vault|Debug")
    ETraversalDebugType VaultSurfaceValidationDebugType = ETraversalDebugType::None;

    UPROPERTY(EditDefaultsOnly, Category="Vault|Debug")
    FColor VaultSurfaceValidationDebugTraceColor = FColor::Red;
    
    UPROPERTY(EditDefaultsOnly, Category="Vault|Debug")
    FColor VaultSurfaceValidationDebugHitColor = FColor::Green;

    UPROPERTY(EditDefaultsOnly, Category="Vault|Debug")
    float VaultSurfaceValidationDebugTime = 3.f;
    
public:

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    void TriggerTraversalAction(bool JumpAction);
    void GridScan(int GridWidth, int GridHeight, const FVector& ScanBaseLocation, const FRotator& ScanRotation);
    void PlayTraversalMontage();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayTraversalMontage(
        FGameplayTag ActionTag,
        FVector TopResultWarpLocation,
        FVector BalanceWarpLocation,
        FVector DepthResultWarpLocation,
        FVector VaultResultWarpLocation,
        FRotator WallRot,
        FHitResult NextClimb
    );

    UFUNCTION(NetMulticast, Reliable)
    void MulticastJump();
    
    void AddMovementInput(float ScaleValue, bool Front);

    void ResetMovement();

    UFUNCTION(Server, Reliable)
    void ServerResetMovement();
    
    void DropFromClimb();

    UFUNCTION(Server, Reliable)
    void ServerDropFromClimb();

    UFUNCTION(Server, Reliable)
    void ServerTriggerTraversalAction(bool bJumpAction);

    UFUNCTION(BlueprintCallable)
    FGameplayTag GetCurrentTraversalState() const { return TraversalState; };
    UFUNCTION(BlueprintCallable)
    FGameplayTag GetCurrentTraversalAction() const { return TraversalAction; };

private:
    UPROPERTY(ReplicatedUsing=OnRep_TraversalState)
    FGameplayTag TraversalState = FUmbraGameplayTags::Get().Traversal_State_FreeRoam;
    UPROPERTY(ReplicatedUsing=OnRep_TraversalAction)
    FGameplayTag TraversalAction = FUmbraGameplayTags::Get().Traversal_Action_NoAction;
    UPROPERTY(ReplicatedUsing=OnRep_ClimbStyle)
    FGameplayTag ClimbStyle = FUmbraGameplayTags::Get().Traversal_ClimbStyle_BracedClimb;
    UPROPERTY(ReplicatedUsing=OnRep_ClimbDirection)
    FGameplayTag ClimbDirection = FUmbraGameplayTags::Get().Traversal_Direction_NoDirection;
    
    FHitResult WallHitResult = FHitResult();
    FHitResult WallEdgeResult = FHitResult();
    FHitResult WallTopResult = FHitResult();
    FHitResult WallDepthResult = FHitResult();
    FHitResult WallVaultResult = FHitResult();
    FHitResult NextClimbResult = FHitResult();
    FHitResult CurrentClimbResult = FHitResult();
    
    FRotator WallRotation = FRotator();
    float WallHeight;
    float WallDepth;
    float VaultHeight;
    
    bool bInLand = true;

    UPROPERTY(Replicated)
    bool bIsDropping = false;

    FTraversalActionData CurrentActionData;
    
    TObjectPtr<ACharacter> OwnerCharacter;
    TObjectPtr<UCharacterMovementComponent> CharacterMovement;
    TObjectPtr<UMotionWarpingComponent> MotionWarping;
    TObjectPtr<UCameraComponent> Camera;
    TObjectPtr<UCapsuleComponent> Capsule;
    TObjectPtr<USkeletalMeshComponent> SkeletalMesh;
    TObjectPtr<UAnimInstance> AnimInstance;
    
    float ForwardValue;
    float RightValue;

    FUmbraGameplayTags UGT = FUmbraGameplayTags::Get();

protected:

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    
    void InitializeReferences();

    /* Setters */
    void SetTraversalStateSettings(ECollisionEnabled::Type CollisionType, EMovementMode MovementMode, const bool& StopMovementImmediately);
    void SetTraversalState(const FGameplayTag& NewTraversalState);
    void SetClimbStyle(const FGameplayTag& NewClimbStyle);
    void SetClimbDirection (const FGameplayTag& NewClimbDirection);
    void SetTraversalAction(const FGameplayTag& NewTraversalAction);

    UFUNCTION()
    void OnRep_TraversalState();
    UFUNCTION()
    void OnRep_TraversalAction();
    UFUNCTION()
    void OnRep_ClimbStyle();
    UFUNCTION()
    void OnRep_ClimbDirection();
    
    /* Wall Detection */
    FHitResult DetectWall() const;
    bool FindWallEdge(int GridWidth, int GridHeight, const FVector& ScanBaseLocation, const FRotator& ScanRotation, FHitResult& OutWallEdgeResult);
    bool FindWallTop(const FHitResult& WallEdgeHit, const FRotator& WallRot, FHitResult& OutWallTopResult, FHitResult& OutLastTopHit, bool& OutEndOfWallFound);
    bool FindWallDepth(const FHitResult& LastTopHit, const FRotator& WallRot, FHitResult& OutWallDepthResult);
    bool FindWallVault(const FHitResult& WallDepthHit, const FRotator& WallRot, FHitResult& OutWallVaultResult);
    FVector FindWarpLocation(const FVector& Location, const FRotator& Rotation, float XOffset, float ZOffset) const;
    void MeasureWall();
    
    void DecideTraversalType(bool JumpAction);
    void DecideClimbStyle(const FVector& Location, const FRotator& Rotation);
    void DecideClimbOrHope();

    void ClimbMovement();

    UFUNCTION(Server, Reliable)
    void ServerSendClimbInput(float FValue, float RValue);
    
    void StopClimbMovement();

    UFUNCTION(Server, Reliable)
    void ServerStopClimbMovement();
    
    bool ClimbCheckForSides(const FVector& ImpactPoint);
    void SetNewClimbPosition(float NewLocationX, float NewLocationY, float NewLocationZ, FRotator NewRotation);
    bool ValidateClimbSurface(const FVector& Location, const FRotator& Rotation) const;
    bool ValidateClimbMovementSurface(const FVector& MovementImpactLocation) const;

    void ValidateIsInLand();
    bool ValidateAirHang() const;
    
    void ClimbMovementIK();
    UFUNCTION(BlueprintCallable)
    void NextClimbHandIK(const bool bLeftHand);
    UFUNCTION(BlueprintCallable)
    void NextClimbFootIK(const bool bLeftFoot);
    UFUNCTION(BlueprintCallable)
    void UpdateHandIK(const bool bLeftHand);
    UFUNCTION(BlueprintCallable)
    void UpdateFootIK(const bool bLeftFoot);
    void ResetFootIK() const;

    float GetVerticalHopDistance() const;
    float GetHorizontalHopDistance() const;
    FGameplayTag DecideHopAction() const;
    void FindHopLocation();

    bool ValidateMantleSurface() const;
    bool ValidateVaultPath() const;
    bool ValidateVaultSurface() const;

    /* Helper Functions */
    float ClimbValues (const FGameplayTag& NewClimbStyle, float Braced, float FreeHang) const;
    float DirectionValues(const FGameplayTag& Direction, float Forward, float ForwardRight, float Right,
        float BackwardRight, float Backward, float BackwardLeft, float Left, float ForwardLeft) const;
    float TraversalStateValues(const FGameplayTag& State, float FreeRoam, float ReadyToClimb, float Climb, float Mantle, float Vault) const;
    FVector VectorDirectionMove (const FVector& Source, const FGameplayTag& Direction, const float& MoveValue) const;
    FVector VectorDirectionMoveWithRotation (const FVector& Source, const FGameplayTag& Direction, const float& MoveValue, const FRotator& Rotation) const;
    FRotator ReverseNormal (const FVector& Normal);
    FGameplayTag GetControllerDirection() const;
    float GetCharacterHandHeight() const;
    void ResetTraversalResults();
    EDrawDebugTrace::Type GetDebugType(ETraversalDebugType DebugValue) const;
};



