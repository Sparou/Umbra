// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UmbraGameplayTags.h"
#include "GameFramework/PlayerController.h"
#include "UmbraPlayerController.generated.h"

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
	UPROPERTY(BlueprintReadOnly)
	bool bWantsToInteract = false;
	
protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InputContext;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, Category = "Input|Characters")
	TObjectPtr<UInputAction> SwitchToAssassinAction;

	UPROPERTY(EditAnywhere, Category = "Input|Characters")
	TObjectPtr<UInputAction> SwitchToTrapperAction;
	
	// UPROPERTY(EditAnywhere, Category = "Input")
	// TObjectPtr<UInputAction> SwitchToSniperAction;
	//
	// UPROPERTY(EditAnywhere, Category = "Input")
	// TObjectPtr<UInputAction> SwitchToFacelessAction;
	
	UPROPERTY(EditAnywhere, Category = "Characters")
	TArray<TSubclassOf<AUmbraPlayerCharacter>> OwnedCharacters;

	UPROPERTY(EditDefaultsOnly, Category = "Characters")
	TObjectPtr<UPlayerCharacterInfo> PlayerCharactersInfo;

	void SwitchCharacter(FGameplayTag CharacterTag);
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	void Interact(bool WantsToInteract);


};
