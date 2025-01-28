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

	/** Traversal Tags */
	FGameplayTag Traversal_State_FreeRoam;
	FGameplayTag Traversal_State_ReadyToClimb;
	FGameplayTag Traversal_State_Climb;
	FGameplayTag Traversal_State_Mantle;
	FGameplayTag Traversal_State_Vault;
	FGameplayTag Traversal_Action_NoAction;
	FGameplayTag Traversal_Action_BracedClimb;
	FGameplayTag Traversal_Action_BracedClimb_FallingClimb;
	FGameplayTag Traversal_Action_BracedClimb_ClimbUp;
	FGameplayTag Traversal_Action_BracedClimb_HopUp;
	FGameplayTag Traversal_Action_BracedClimb_HopLeft;
	FGameplayTag Traversal_Action_BracedClimb_HopRight;
	FGameplayTag Traversal_Action_BracedClimb_HopLeftUp;
	FGameplayTag Traversal_Action_BracedClimb_HopRightUp;
	FGameplayTag Traversal_Action_BracedClimb_HopDown;
	FGameplayTag Traversal_Action_FreeHang;
	FGameplayTag Traversal_Action_FreeHang_FallingClimb;
	FGameplayTag Traversal_Action_FreeHang_ClimbUp;
	FGameplayTag Traversal_Action_FreeHang_HopLeft;
	FGameplayTag Traversal_Action_FreeHang_HopRight;
	FGameplayTag Traversal_Action_FreeHang_HopDown;
	FGameplayTag Traversal_Action_CornerMove;
	FGameplayTag Traversal_Action_Mantle;
	FGameplayTag Traversal_Action_Vault;
	FGameplayTag Traversal_Direction_NoDirection;
	FGameplayTag Traversal_Direction_Left;
	FGameplayTag Traversal_Direction_Right;
	FGameplayTag Traversal_Direction_Forward;
	FGameplayTag Traversal_Direction_Backward;
	FGameplayTag Traversal_Direction_ForwardLeft;
	FGameplayTag Traversal_Direction_ForwardRight;
	FGameplayTag Traversal_Direction_BackwardLeft;
	FGameplayTag Traversal_Direction_BackwardRight;
	FGameplayTag Traversal_ClimbStyle_BracedClimb;
	FGameplayTag Traversal_ClimbStyle_FreeHang;
		
private:
	static FUmbraGameplayTags GameplayTags;
};
