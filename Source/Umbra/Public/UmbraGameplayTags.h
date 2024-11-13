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

private:
	static FUmbraGameplayTags GameplayTags;
};
