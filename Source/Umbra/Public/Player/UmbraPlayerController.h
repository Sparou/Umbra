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

	UFUNCTION(BlueprintCallable)
	void SwitchToDefaultContext();
	
	UFUNCTION(BlueprintCallable)
	void SwitchToCameraOnlyContext();

	UFUNCTION(BlueprintCallable)
	void SwitchToClimbContext();
	
protected:

	virtual void BeginPlay() override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputMappingContext> InputContext;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input|Abilities")
	TObjectPtr<UInputAction> ConfirmAction;
	
	UPROPERTY(EditAnywhere, Category = "Input|Abilities")
	TObjectPtr<UInputAction> CancelAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> DropAction;
	
	UPROPERTY(EditAnywhere, Category = "Input|Camera")
	TObjectPtr<UInputAction> CameraZoomAction;

	UPROPERTY(EditAnywhere, Category = "Input|Camera")
	TObjectPtr<UInputMappingContext> CameraContext;
	
	UPROPERTY(EditAnywhere, Category = "Input|Camera")
	float CameraZoomStep = 10.f;

	UPROPERTY(EditAnywhere, Category = "Input|Camera")
	float MinCameraZoom = 150.f;

	UPROPERTY(EditAnywhere, Category = "Input|Camera")
	float MaxCameraZoom = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input|Abilities")
	TObjectPtr<UUmbraInputConfig> InputConfig;
	
	UPROPERTY(EditAnywhere, Category = "Input|Climb")
	TObjectPtr<UInputMappingContext> ClimbContext;
	
	UPROPERTY(EditAnywhere, Category = "Input|Climb")
	TObjectPtr<UInputAction> ClimbMoveAction;

	UPROPERTY(EditAnywhere, Category = "Input|Climb")
	TObjectPtr<UInputAction> ClimbDropAction;
	
private:
	
	TWeakObjectPtr<AUmbraBaseCharacter> UmbraCharacter;
	TWeakObjectPtr<UUmbraAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UAnimInstance> AnimInstance;
	TWeakObjectPtr<USpringArmComponent> SpringArmComponent;

	UUmbraAbilitySystemComponent* GetAbilitySystemComponent();
	UAnimInstance* GetAnimInstance();
	AUmbraBaseCharacter* GetUmbraCharacter();
	USpringArmComponent* GetSpingArmComponent();
	
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	void CameraZoom(const FInputActionValue& InputActionValue);

	void ClimbMove(const FInputActionValue& InputActionValue);
	void ClimbDrop();

	void ConfirmAbilityTargeting();
	void CancelAbilityTargeting();
	
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
};
