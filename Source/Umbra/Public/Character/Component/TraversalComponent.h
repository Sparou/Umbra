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
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	

private:
	
    void InitializeReferences();
	void SetTraversalStateSettings(ECollisionEnabled::Type CollisionType, EMovementMode MovementMode, const bool& StopMovementImmediately);
	void SetTraversalState(const FGameplayTag& NewTraversalState);
	void SetClimbStyle(const FGameplayTag& NewClimbStyle);
	void SetClimbDirection (const FGameplayTag& NewClimbDirection);
	float ClimbValues (const FGameplayTag& NewClimbStyle, const float& Braced, const float& FreeHang);
	
	FGameplayTag TraversalState = FUmbraGameplayTags::Get().Traversal_State_FreeRoam;
	FGameplayTag ClimbStyle = FUmbraGameplayTags::Get().Traversal_ClimbStyle_BracedClimb;
	FGameplayTag ClimbDirection = FUmbraGameplayTags::Get().Traversal_Direction_NoDirection;
	
	/** Components */
	TObjectPtr<ACharacter> OwnerCharacter;
	TObjectPtr<UCharacterMovementComponent> CharacterMovement;
	TObjectPtr<UMotionWarpingComponent> MotionWarping;
	TObjectPtr<UCameraComponent> Camera;
	TObjectPtr<UCapsuleComponent> Capsule;
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;
	TObjectPtr<UAnimInstance> AnimInstance;
		
};
