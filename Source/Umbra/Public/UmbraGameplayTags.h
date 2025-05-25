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
	FGameplayTag Input_T;
	
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
	FGameplayTag Ability_Through;
	FGameplayTag Ability_Invisibility;
	FGameplayTag Ability_DomeSound;
	FGameplayTag Ability_TrapBlock;
	FGameplayTag Ability_TrapTeleport;
	FGameplayTag Ability_TrapKill;
	
private:
	static FUmbraGameplayTags GameplayTags;
};
