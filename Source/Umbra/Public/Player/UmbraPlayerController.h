// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UmbraGameplayTags.h"
#include "GameFramework/PlayerController.h"
#include "UmbraPlayerController.generated.h"

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
	

	UFUNCTION(BlueprintCallable)
	void SwitchToDefaultContext();
	
	UFUNCTION(BlueprintCallable)
	void SwitchToCameraOnlyContext();

	UFUNCTION(BlueprintCallable)
	void SwitchToArrowContext();
	
protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:

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

	UUmbraAbilitySystemComponent* GetAbilitySystemComponent();
	UTraversalComponent* GetTraversalComponent();
	UAnimInstance* GetAnimInstance();
	UTagManager* GetTagManager();
	AUmbraBaseCharacter* GetControlledCharacter();
	UInteractionComponent* GetInteractionComponent();
	
	void SwitchCharacter(FGameplayTag CharacterTag);
	void Interact();

	void DirectArrow(const FInputActionValue& InputActionValue);

	
	void OnInteract();
	void Interact(AActor* InteractionTarget);
	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* InteractionTarget);
	
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);

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
	
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	void SetWalking(bool bWalking);
	UFUNCTION(Server, Reliable)
	void ServerSetWalking(bool bWalking);
};
