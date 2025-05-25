// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Player/UmbraPlayerController.h"
#include "UmbraForShadowPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API AUmbraForShadowPlayerController : public AUmbraPlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<AUmbraShadowController*> ShadowControllers;

	UPROPERTY(BlueprintReadWrite)
	TArray<AUmbraLineShadowController*> LineShadowControllers;

	// virtual void SetupInputComponent();
	
	// virtual void Move(const FInputActionValue& InputActionValue) override;
	// virtual void Look(const FInputActionValue& InputActionValue) override;
	//
	// virtual void OnStartWalking() override;
	// virtual void OnStopWalking() override;
	//
	// virtual void OnStartJumping() override;
	// virtual void OnStopJumping() override;
	//
	// virtual void OnStartCrouch() override;
	// virtual void OnStopCrouch() override;
	//
	// virtual void OnStartThrough() override;
	// virtual void OnStopThrough() override;
	//
	// virtual void AbilityInputTagPressed(FGameplayTag InputTag) override;
	// virtual void AbilityInputTagReleased(FGameplayTag InputTag) override;
	// virtual void AbilityInputTagHeld(FGameplayTag InputTag) override;
};
