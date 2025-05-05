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

	GameplayTags.State_Locomotion_Walking = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("State.Locomotion.Walking"));
	GameplayTags.State_Locomotion_Running = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("State.Locomotion.Running"));
	GameplayTags.State_Movement_Moving = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("State.Movement.Moving"));
	GameplayTags.State_Movement_Falling = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("State.Movement.Falling"));
	GameplayTags.State_Stance_Standing = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("State.Stance.Standing"));
	GameplayTags.State_Stance_Crouching = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("State.Stance.Crouching"));

	/** Position Tags */
	GameplayTags.Position_Front = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Position.Front"));
	GameplayTags.Position_FrontRight = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Position.FrontRight"));
	GameplayTags.Position_Right = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Position.Right"));
	GameplayTags.Position_BehindRight = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Position.BehindRight"));
	GameplayTags.Position_Behind = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Position.Behind"));
	GameplayTags.Position_BehindLeft = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Position.BehindLeft"));
	GameplayTags.Position_Left = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Position.Left"));
	GameplayTags.Position_FrontLeft = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Position.FrontLeft"));
	
	/** Traversal Tags */
	GameplayTags.Traversal_State_FreeRoam = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.State.FreeRoam"));
	GameplayTags.Traversal_State_ReadyToClimb = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.State.ReadyToClimb"));
	GameplayTags.Traversal_State_Climb = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.State.Climb"));
	GameplayTags.Traversal_State_Mantle = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.State.Mantle"));
	GameplayTags.Traversal_State_Vault = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.State.Vault"));
	GameplayTags.Traversal_Action_NoAction = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.NoAction"));
	GameplayTags.Traversal_Action_BracedClimb = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.BracedClimb"));
	GameplayTags.Traversal_Action_BracedClimb_FallingClimb = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.BracedClimb.FallingClimb"));
	GameplayTags.Traversal_Action_BracedClimb_ClimbUp = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.BracedClimb.ClimbUp"));
	GameplayTags.Traversal_Action_BracedClimb_HopUp = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.BracedClimb.HopUp"));
	GameplayTags.Traversal_Action_BracedClimb_HopLeft = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.BracedClimb.HopLeft"));
	GameplayTags.Traversal_Action_BracedClimb_HopRight = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.BracedClimb.HopeRight"));
	GameplayTags.Traversal_Action_BracedClimb_HopLeftUp = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.BracedClimb.HopeLeftUp"));
	GameplayTags.Traversal_Action_BracedClimb_HopRightUp = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.BracedClimb.HopRightUp"));
	GameplayTags.Traversal_Action_BracedClimb_HopDown = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.BracedClimb.HopDown"));
	GameplayTags.Traversal_Action_FreeHang = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.FreeHang"));
	GameplayTags.Traversal_Action_FreeHang_FallingClimb = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.FreeHang.FallingClimb"));
	GameplayTags.Traversal_Action_FreeHang_ClimbUp = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.FreeHang.ClimbUp"));
	GameplayTags.Traversal_Action_FreeHang_HopUp = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.FreeHang.HopUp"));
	GameplayTags.Traversal_Action_FreeHang_HopLeft = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.FreeHang.HopLeft"));
	GameplayTags.Traversal_Action_FreeHang_HopRight = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.FreeHang.HopRight"));
	GameplayTags.Traversal_Action_FreeHang_HopLeftUp = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.FreeHang.HopLeftUp"));
	GameplayTags.Traversal_Action_FreeHang_HopRightUp = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.FreeHang.HopRightUp"));
	GameplayTags.Traversal_Action_FreeHang_HopDown = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.FreeHang.HopDown"));
	GameplayTags.Traversal_Action_CornerMove = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.CornerMove"));
	GameplayTags.Traversal_Action_Mantle = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.Mantle"));
	GameplayTags.Traversal_Action_Vault = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Action.Vault"));
	GameplayTags.Traversal_Direction_NoDirection = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Direction.NoDirection"));
	GameplayTags.Traversal_Direction_Up = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Direction.Up"));
	GameplayTags.Traversal_Direction_Down = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Direction.Down"));
	GameplayTags.Traversal_Direction_Left = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Direction.Left"));
	GameplayTags.Traversal_Direction_Right = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Direction.Right"));
	GameplayTags.Traversal_Direction_Forward = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Direction.Forward"));
	GameplayTags.Traversal_Direction_Backward = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Direction.Backward"));
	GameplayTags.Traversal_Direction_ForwardLeft = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Direction.ForwardLeft"));
	GameplayTags.Traversal_Direction_ForwardRight = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Direction.ForwardRight"));
	GameplayTags.Traversal_Direction_BackwardLeft = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Direction.BackwardLeft"));
	GameplayTags.Traversal_Direction_BackwardRight = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Direction.BackwardRight"));
	GameplayTags.Traversal_ClimbStyle_BracedClimb = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.ClimbStyle.BracedClimb"));
	GameplayTags.Traversal_ClimbStyle_FreeHang = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.ClimbStyle.FreeHang"));
}
