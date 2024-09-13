// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UmbraGameplayTags.h"
#include "GameFramework/PlayerController.h"
#include "UmbraPlayerController.generated.h"

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

protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InputContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SwitchCharacterAction;

	UPROPERTY(EditAnywhere, Category = "Characters")
	TArray<TSubclassOf<AUmbraBaseCharacter>> OwnedCharacters;

	UPROPERTY(EditDefaultsOnly, Category = "Characters")
	TObjectPtr<UPlayerCharacterInfo> PlayerCharactersInfo;

	void SwitchCharacter(FGameplayTag CharacterTag);

	void SwitchToAssassin() { SwitchCharacter(FUmbraGameplayTags::Get().Character_Assassin); }
	void SwtichToTrapper() { SwitchCharacter(FUmbraGameplayTags::Get().Character_Trapper); }


};
