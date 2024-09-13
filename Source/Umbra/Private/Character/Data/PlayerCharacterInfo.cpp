// Copyrighted by Vorona Games


#include "Character/Data/PlayerCharacterInfo.h"

#include "Character/UmbraPlayerCharacter.h"

TSubclassOf<AUmbraPlayerCharacter> UPlayerCharacterInfo::FindPlayerCharacterBlueprintByTag(
	FGameplayTag CharacterTag, bool bLogNotFound)
{
	for (const FUmbraPlayerCharacterInfo& CharacterInfo : PlayerCharactersInfo)
	{
		if (CharacterInfo.CharacterTag == CharacterTag)
		{
			return CharacterInfo.CharacterBlueprint;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find character blueprint [%s] on PlayerCharactersInfo [%s]"),
			*CharacterTag.ToString(), *GetNameSafe(this))
	}

	return TSubclassOf<AUmbraPlayerCharacter>();
}
