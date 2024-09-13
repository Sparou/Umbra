// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "PlayerCharacterInfo.generated.h"

class AUmbraPlayerCharacter;

USTRUCT(BlueprintType)
struct FUmbraPlayerCharacterInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText CharacterName = FText();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CharacterTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AUmbraPlayerCharacter> CharacterBlueprint;
};

/**
 * 
 */
 
UCLASS()
class UMBRA_API UPlayerCharacterInfo : public UDataAsset
{
	GENERATED_BODY()

public:

	TSubclassOf<AUmbraPlayerCharacter> FindPlayerCharacterBlueprintByTag(FGameplayTag CharacterTag, bool bLogNotFound = false);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FUmbraPlayerCharacterInfo> PlayerCharactersInfo;
};
