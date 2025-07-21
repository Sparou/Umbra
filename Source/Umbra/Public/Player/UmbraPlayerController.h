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
	
protected:

	virtual void BeginPlay() override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InputContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> CameraOnlyInputContext;

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
	float CameraZoomStep = 10.f;

	UPROPERTY(EditAnywhere, Category = "Input|Camera")
	float MinCameraZoom = 150.f;

	UPROPERTY(EditAnywhere, Category = "Input|Camera")
	float MaxCameraZoom = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Abilities")
	TObjectPtr<UUmbraInputConfig> InputConfig;
	
private:
	
	TWeakObjectPtr<AUmbraBaseCharacter> UmbraCharacter;
	TWeakObjectPtr<UUmbraAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UTraversalComponent> TraversalComponent;
	TWeakObjectPtr<UAnimInstance> AnimInstance;
	TWeakObjectPtr<USpringArmComponent> SpringArmComponent;

	UUmbraAbilitySystemComponent* GetAbilitySystemComponent();
	UTraversalComponent* GetTraversalComponent();
	UAnimInstance* GetAnimInstance();
	AUmbraBaseCharacter* GetUmbraCharacter();
	USpringArmComponent* GetSpingArmComponent();
	
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	void CameraZoom(const FInputActionValue& InputActionValue);
	
	void OnStopMoving();
	void OnStartDrop();

	void ConfirmAbilityTargeting();
	void CancelAbilityTargeting();
	
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
};
