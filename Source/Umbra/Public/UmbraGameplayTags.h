// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
/**
 *  Singleton containing native Gameplay Tags
 */

struct FUmbraGameplayTags
{
public:
	static const FUmbraGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	/** Character Tags */
	FGameplayTag Character_Assassin;
	FGameplayTag Character_Trapper;
	FGameplayTag Character_Sniper;
	FGameplayTag Character_Faceless;

	/** Input Tags */
	FGameplayTag Input_1;
	FGameplayTag Input_2;
	FGameplayTag Input_3;
	FGameplayTag Input_4;
	
	/** Combat Tags */
	FGameplayTag WeaponTrace_Start;
	FGameplayTag WeaponTrace_Tick;
	FGameplayTag WeaponTrace_End;
	
	FGameplayTag Combat_Damage;
	FGameplayTag Combat_HitReact;

	/** Ability Tags */
	FGameplayTag Ability_Melee;
	FGameplayTag Ability_Stealth_Kill;
	FGameplayTag Ability_Stealth_Victim;

	/** AI Event Tags */
	FGameplayTag AI_Event_HearNoise;
	FGameplayTag AI_Event_SeeAlly;
	FGameplayTag AI_Event_SeeEnemy;
	FGameplayTag AI_Event_TakeDamage;
	
private:
	static FUmbraGameplayTags GameplayTags;
};
