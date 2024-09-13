// Copyrighted by Vorona Games


#include "UmbraGameplayTags.h"
#include "GameplayTagsManager.h"

FUmbraGameplayTags FUmbraGameplayTags::GameplayTags;

void FUmbraGameplayTags::InitializeNativeGameplayTags()
{
	GameplayTags.Character_Assassin = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Character.Assassin"));
	GameplayTags.Character_Trapper = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Character.Trapper"));
	GameplayTags.Character_Sniper = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Character.Sniper"));
	GameplayTags.Character_Faceless = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Character.Faceless"));
}
