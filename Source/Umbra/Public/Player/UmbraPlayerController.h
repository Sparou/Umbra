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
	
protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InputContext;
	
	UPROPERTY(EditAnywhere, Category = "Input|Basic")
	TObjectPtr<UInputAction> InteractAction;
	
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

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
};
