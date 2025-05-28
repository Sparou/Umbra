// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UmbraLineShadowController.generated.h"


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
class UMBRA_API AUmbraLineShadowController : public APlayerController
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Umbra|Shadow")
	float FixedOffset = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Umbra|Shadow")
	AUmbraPlayerCharacter* MasterCharacter = nullptr;	

	UPROPERTY()
	TObjectPtr<UUmbraAbilitySystemComponent> AbilitySystemComponent;
	
	void UpdateCloneOffset();
	void Tick(float DeltaTime);
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
