// Copyrighted by Vorona Games


#include "UmbraGameplayTags.h"
#include "GameplayTagsManager.h"

FUmbraGameplayTags FUmbraGameplayTags::GameplayTags;

void FUmbraGameplayTags::InitializeNativeGameplayTags()
{
	/** Сharacter Tags */
	GameplayTags.Character_Assassin = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Character.Assassin"));
	GameplayTags.Character_Trapper = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Character.Trapper"));
	GameplayTags.Character_Sniper = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Character.Sniper"));
	GameplayTags.Character_Faceless = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Character.Faceless"));

	/** Input Tags */
	GameplayTags.Input_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.1"));
	GameplayTags.Input_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.2"));
	GameplayTags.Input_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.3"));
	GameplayTags.Input_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.4"));
	GameplayTags.Input_T = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.T"));

	/** Combat Tags */
	GameplayTags.WeaponTrace_Start = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("WeaponTrace.Start"));
	GameplayTags.WeaponTrace_Tick = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("WeaponTrace.Tick"));
	GameplayTags.WeaponTrace_End = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("WeaponTrace.End"));

	GameplayTags.Combat_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Damage"));
	GameplayTags.Combat_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.HitReact"));

	/** Ability Tags */
	GameplayTags.Ability_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Melee"));
	GameplayTags.Ability_Stealth_Kill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Stealth.Kill"));
	GameplayTags.Ability_Stealth_Victim = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Stealth.Victim"));
	GameplayTags.Ability_Through = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Through"));
	GameplayTags.Ability_Invisibility = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Invisibility"));
	GameplayTags.Ability_Invisibility = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Trap.Block"));
	GameplayTags.Ability_Invisibility = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Trap.Kill"));
	GameplayTags.Ability_Invisibility = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Trap.Teleport"));
} 
