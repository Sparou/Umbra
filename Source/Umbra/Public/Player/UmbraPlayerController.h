// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UmbraGameplayTags.h"
#include "GameFramework/PlayerController.h"
#include "UmbraPlayerController.generated.h"

class USpringArmComponent;
class UInteractionComponent;
class UTraversalComponent;
class UUmbraAbilitySystemComponent;
class UUmbraInputConfig;
struct FInputActionValue;
struct FUmbraPlayerCharacterInfo;
class UPlayerCharacterInfo;
struct FGameplayTag;
class AUmbraBaseCharacter;
class AUmbraPlayerCharacter;
class UInputMappingContext;
class UInputAction;
class UTagManager;

/**
 * 
 */
UCLASS()
class UMBRA_API AUmbraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToWalk;

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToJump;

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToCrouch;

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToThough;

	UFUNCTION(BlueprintCallable)
	void SwitchToDefaultContext();
	
	UFUNCTION(BlueprintCallable)
	void SwitchToCameraOnlyContext();

	UFUNCTION(BlueprintCallable)
	void SwitchToArrowContext();
	
protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InputContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> CameraOnlyInputContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> ArrowContext;
	
	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> ArrowAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> WalkAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> DropAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> PauseAction;
	
	UPROPERTY(EditAnywhere, Category = "Input|Camera")
	TObjectPtr<UInputAction> CameraZoomAction;

	UPROPERTY(EditAnywhere, Category = "Input|Camera")
	float CameraZoomStep = 10.f;

	UPROPERTY(EditAnywhere, Category = "Input|Camera")
	float MinCameraZoom = 150.f;

	UPROPERTY(EditAnywhere, Category = "Input|Camera")
	float MaxCameraZoom = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Abilities")
	TObjectPtr<UUmbraInputConfig> InputConfig;
	
private:
	
	UPROPERTY()
	TObjectPtr<AUmbraBaseCharacter> ControlledCharacter;
	UPROPERTY()
	TObjectPtr<UUmbraAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UTraversalComponent> TraversalComponent;
	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance;
	UPROPERTY()
	TObjectPtr<UTagManager> TagManager;
	UPROPERTY()
	TObjectPtr<UInteractionComponent> InteractionComponent;
	UPROPERTY()
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UUmbraAbilitySystemComponent* GetAbilitySystemComponent();
	UTraversalComponent* GetTraversalComponent();
	UAnimInstance* GetAnimInstance();
	UTagManager* GetTagManager();
	AUmbraBaseCharacter* GetControlledCharacter();
	UInteractionComponent* GetInteractionComponent();
	USpringArmComponent* GetSpingArmComponent();
	
	void SwitchCharacter(FGameplayTag CharacterTag);
	void Interact();

	void DirectArrow(const FInputActionValue& InputActionValue);
	
	void OnInteract();
	void Interact(AActor* InteractionTarget);
	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* InteractionTarget);
	
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	void CameraZoom(const FInputActionValue& InputActionValue);

	void OnStartMoving();
	void OnStopMoving();
	
	void OnStartWalking();
	void OnStopWalking();

	void OnStartJumping();
	void OnStopJumping();

	void OnStartCrouch();
	void OnStopCrouch();

	void OnStartDrop();

	void OnStartThrough();
	void OnStopThrough();
	
	virtual void Pause() override;

	UFUNCTION(BlueprintCallable)
	void OnPause(bool bIsPaused);

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	void SetWalking(bool bWalking);
	UFUNCTION(Server, Reliable)
	void ServerSetWalking(bool bWalking);
};
