// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UmbraGameplayTags.h"
#include "GameFramework/PlayerController.h"
#include "UmbraPlayerController.generated.h"

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
class UMBRA_API AUmbraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> OverlapedActor = nullptr;

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToWalk;

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToJump;

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToCrouch;

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToThough;
	
	void SwitchToDefaultContext();
	void SwitchToCameraOnlyContext();
	
protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InputContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> CameraOnlyInputContext;
	
	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> WalkAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> CrouchAction;
	
	UPROPERTY(EditAnywhere, Category = "Input|Characters")
	TObjectPtr<UInputAction> SwitchToAssassinAction;

	UPROPERTY(EditAnywhere, Category = "Input|Characters")
	TObjectPtr<UInputAction> SwitchToTrapperAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Abilities")
	TObjectPtr<UUmbraInputConfig> InputConfig;
	
	UPROPERTY(EditAnywhere, Category = "Characters")
	TArray<TSubclassOf<AUmbraPlayerCharacter>> OwnedCharacters;

	UPROPERTY(EditDefaultsOnly, Category = "Characters")
	TObjectPtr<UPlayerCharacterInfo> PlayerCharactersInfo;

	UPROPERTY()
	TObjectPtr<UUmbraAbilitySystemComponent> AbilitySystemComponent;

	UUmbraAbilitySystemComponent* GetAbilitySystemComponent();
	
	void SwitchCharacter(FGameplayTag CharacterTag);
	void Interact();
	
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	
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
