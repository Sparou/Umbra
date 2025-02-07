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

	void TriggerTraversalAction(const bool& JumpAction);
	void GridScan(int GridWidth, int GridHeight, const FVector& ScanBaseLocation, const FRotator& ScanRotation);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	

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
