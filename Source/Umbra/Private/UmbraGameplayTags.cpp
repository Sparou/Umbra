// Copyrighted by Vorona Games


#include "UmbraGameplayTags.h"
#include "GameplayTagsManager.h"

FUmbraGameplayTags FUmbraGameplayTags::GameplayTags;

void FUmbraGameplayTags::InitializeNativeGameplayTags()
{
	/** Сharacter Tags */
	GameplayTags.Character_Player = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Character.Player"));
	GameplayTags.Character_Enemy = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Character.Enemy"));
	GameplayTags.Character_Assassin = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Character.Assassin"));
	GameplayTags.Character_Trapper = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Character.Trapper"));
	GameplayTags.Character_Sniper = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Character.Sniper"));
	GameplayTags.Character_Faceless = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Character.Faceless"));

	/** Input Tags */
	GameplayTags.Input_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.LMB"));
	GameplayTags.Input_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.RMB"));
	GameplayTags.Input_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.1"));
	GameplayTags.Input_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.2"));
	GameplayTags.Input_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.3"));
	GameplayTags.Input_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.4"));
	GameplayTags.Input_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.Q"));
	GameplayTags.Input_E = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.E"));
	GameplayTags.Input_R = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.R"));
	GameplayTags.Input_T = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.T"));
	GameplayTags.Input_F = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.F"));
	GameplayTags.Input_Shift = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.Shift"));
	GameplayTags.Input_Space = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.Space"));
	GameplayTags.Input_Ctrl = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.Ctrl"));

	/** Combat Tags */
	GameplayTags.WeaponTrace_Start = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("WeaponTrace.Start"));
	GameplayTags.WeaponTrace_Tick = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("WeaponTrace.Tick"));
	GameplayTags.WeaponTrace_End = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("WeaponTrace.End"));

	GameplayTags.Combat_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Damage"));
	GameplayTags.Combat_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.HitReact"));
	GameplayTags.Combat_SpawnProjectile = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.SpawnProjectile"));

	/** Ability Tags */
	GameplayTags.Ability_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Melee"));
	GameplayTags.Ability_Distant = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Distant"));
	GameplayTags.Ability_Stealth_Kill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Stealth.Kill"));
	GameplayTags.Ability_Stealth_Victim = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Stealth.Victim"));
	GameplayTags.Ability_Interact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Interact"));

	// ~ Combat
	GameplayTags.Ability_Combat_Attack_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Combat.Attack.Melee"));
	GameplayTags.Ability_Combat_Attack_Distant = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Combat.Attack.Distant"));

	// ~ Stealth
	GameplayTags.Ability_Stealth_Assassination_Instigator = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Stealth.Assassination.Instigator"));
	GameplayTags.Ability_Stealth_Assassination_Target = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Stealth.Assassination.Target"));
	GameplayTags.Ability_Stealth_Invisibility = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Stealth.Invisibility"));

	// ~ Movement
	GameplayTags.Ability_Movement_Jump = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Movement.Jump"));
	GameplayTags.Ability_Movement_Crouch = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Movement.Crouch"));
	GameplayTags.Ability_Movement_Walk = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Movement.Walk"));
	GameplayTags.Ability_Movement_Dash = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Movement.Dash"));
	GameplayTags.Ability_Movement_Teleport = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Movement.Teleport"));

	GameplayTags.Ability_Movement_Traversal_Climb = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Movement.Traversal.Climb"));
	GameplayTags.Ability_Movement_Traversal_Mantle = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Movement.Traversal.Mantle"));
	GameplayTags.Ability_Movement_Traversal_Vault = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Movement.Traversal.Vault"));

	/** Gameplay Cues Tag */
	GameplayTags.GameplayCue_Invisibility = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Invisibility"));
	
	/** Effects Tags */
	GameplayTags.Effect_Attribute_Visibility = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Effect.Attribute.Visibility"));
	
	GameplayTags.State_Locomotion_Walking = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("State.Locomotion.Walking"));
	GameplayTags.State_Locomotion_Running = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("State.Locomotion.Running"));
	GameplayTags.State_Locomotion_Carrying = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("State.Locomotion.Carrying"));
	GameplayTags.State_Moving = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("State.Moving"));
	GameplayTags.State_Falling = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("State.Falling"));
	GameplayTags.State_Stance_Standing = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("State.Stance.Standing"));
	GameplayTags.State_Stance_Crouching = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("State.Stance.Crouching"));

	GameplayTags.Event_Ability_Traversal_Climb_Move = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Ability.Traversal.Climb.Move"));
	GameplayTags.Event_Ability_Traversal_Climb_Drop = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Ability.Traversal.Climb.Drop"));
	
	/** Position Tags */
	GameplayTags.Position_Front = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Position.Front"));
	GameplayTags.Position_FrontRight = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Position.FrontRight"));
	GameplayTags.Position_Right = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Position.Right"));
	GameplayTags.Position_BehindRight = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Position.BehindRight"));
	GameplayTags.Position_Behind = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Position.Behind"));
	GameplayTags.Position_BehindLeft = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Position.BehindLeft"));
	GameplayTags.Position_Left = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Position.Left"));
	GameplayTags.Position_FrontLeft = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Position.FrontLeft"));

	GameplayTags.Direction_X_Forward = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Direction.X.Forward"));
	GameplayTags.Direction_X_Backward = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Direction.X.Backward"));
	GameplayTags.Direction_Y_Right = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Direction.Y.Right"));
	GameplayTags.Direction_Y_Left = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Direction.Y.Left"));
	GameplayTags.Direction_Z_Up = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Direction.Z.Up"));
	GameplayTags.Direction_Z_Down = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Direction.Z.Down"));


	GameplayTags.Traversal_State_Climb = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.State.Climb"));
	GameplayTags.Traversal_State_Mantle = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.State.Mantle"));
	GameplayTags.Traversal_State_Vault = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.State.Vault"));

	GameplayTags.Traversal_Climb = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Climb"));
	
	GameplayTags.Traversal_Vault = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Vault"));
	GameplayTags.Traversal_Vault_Low_Slow = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Vault.Low.Slow"));
	GameplayTags.Traversal_Vault_Low_Fast = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Vault.Low.Fast"));
	GameplayTags.Traversal_Vault_Medium_Slow = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Vault.Medium.Slow"));
	GameplayTags.Traversal_Vault_Medium_Fast = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Vault.Medium.Fast"));
	GameplayTags.Traversal_Vault_High_Slow = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Vault.High.Slow"));
	GameplayTags.Traversal_Vault_High_Fast = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Vault.High.Fast"));

	GameplayTags.Traversal_Mantle = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Mantle"));
	GameplayTags.Traversal_Mantle_Low = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Mantle.Low"));
	GameplayTags.Traversal_Mantle_Medium = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Mantle.Medium"));
	GameplayTags.Traversal_Mantle_High = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.Mantle.High"));
	
	/** Traversal Tags */
	GameplayTags.Traversal_State_FreeRoam = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.State.FreeRoam"));
	GameplayTags.Traversal_State_ReadyToClimb = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Traversal.State.ReadyToClimb"));

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

	/** AI Event Tags */
	GameplayTags.AI_Event_HearNoise = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AI.Event.HearNoise"));
	GameplayTags.AI_Event_SeeAlly = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AI.Event.SeeAlly"));
	GameplayTags.AI_Event_SeeEnemy = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AI.Event.SeeEnemy"));
	GameplayTags.AI_Event_TakeDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("AI.Event.TakeDamage"));
}
