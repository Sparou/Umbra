// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UmbraPlayerController.generated.h"

class AUmbraBaseCharacter;
class AUmbraPlayerCharacter;
class UInputMappingContext;
class UInputAction;

UENUM(BlueprintType)
enum EUmbraCharacters
{
	Assassin,
	Trapper,
	Sniper,
	Faceless
};

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
	TArray<TSubclassOf<AUmbraBaseCharacter>> PlayerCharacters;

	void SwitchCharacter(EUmbraCharacters NewCharacter);
	
};
