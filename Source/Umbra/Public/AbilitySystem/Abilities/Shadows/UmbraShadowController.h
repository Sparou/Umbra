// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UmbraShadowController.generated.h"

class AUmbraShadowController;
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


/**
 * 
 */
UCLASS()
class UMBRA_API AUmbraShadowController : public APlayerController
{
	GENERATED_BODY()

	
public:
	UPROPERTY(BlueprintReadWrite)
	AUmbraShadowController* ShadowController;

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToWalk;

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToJump;

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToCrouch;

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToThough;
	

	UPROPERTY()
	TObjectPtr<UUmbraAbilitySystemComponent> AbilitySystemComponent;

	auto GetAbilitySystemComponent() -> UUmbraAbilitySystemComponent*;
	
	void Move(const FVector ForwardDirection, const float Y, const FVector RightDirection, const float X);
	void Look(const float X, const float Y);
	
	void OnStartWalking();
	void OnStopWalking();

	void OnStartJumping();
	void OnStopJumping();

	void OnStartCrouch();
	void OnStopCrouch();

	void OnStartThrough();
	void OnStopThrough();
	
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
};