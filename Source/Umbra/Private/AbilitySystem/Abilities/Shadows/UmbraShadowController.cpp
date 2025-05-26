// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Shadows/UmbraShadowController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "UmbraGameplayTags.h"
#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Character/Data/PlayerCharacterInfo.h"
#include "GameFramework/CharacterMovementComponent.h"


UUmbraAbilitySystemComponent* AUmbraShadowController::GetAbilitySystemComponent()
{
	if (AbilitySystemComponent == nullptr)
	{
		AbilitySystemComponent = Cast<UUmbraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}

	return AbilitySystemComponent;
}


void AUmbraShadowController::Move(const FVector ForwardDirection, const float Y, const FVector RightDirection, const float X)
{
	//UE_LOG(LogTemp, Warning, TEXT("AUmbraShadowPlayerController::Move: %s"), *InputActionValue.ToString());
	

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, Y);
		ControlledPawn->AddMovementInput(RightDirection, X);
	}
}

void AUmbraShadowController::Look(const float X, const float Y)
{
	APawn* CurrentPawn = GetPawn();
	CurrentPawn->AddControllerYawInput(X);
	CurrentPawn->AddControllerPitchInput(Y);
}

void AUmbraShadowController::OnStartWalking()
{
	UE_LOG(LogTemp, Error, TEXT("AUmbraShadowPlayerController::OnStartWalking"));
	bWantsToWalk = true;
	if (AUmbraBaseCharacter* ControlledCharacter = Cast<AUmbraBaseCharacter>(GetCharacter()))
	{
		ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = ControlledCharacter->GetMoveSpeed(FUmbraGameplayTags::Get().State_Stance_Standing, FUmbraGameplayTags::Get().State_Locomotion_Running);
	}
}

void AUmbraShadowController::OnStopWalking()
{
	bWantsToWalk = false;
	if (AUmbraBaseCharacter* ControlledCharacter = Cast<AUmbraBaseCharacter>(GetCharacter()))
	{
		ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = ControlledCharacter->GetMoveSpeed(FUmbraGameplayTags::Get().State_Stance_Standing, FUmbraGameplayTags::Get().State_Locomotion_Running);
	}
}

void AUmbraShadowController::OnStartJumping()
{
	bWantsToJump = true;
	if (AUmbraBaseCharacter* ControlledCharacter = Cast<AUmbraBaseCharacter>(GetCharacter()))
	{
		ControlledCharacter->Jump();
	}
}

void AUmbraShadowController::OnStopJumping()
{
	bWantsToJump = false;
}

void AUmbraShadowController::OnStartCrouch()
{
	bWantsToCrouch = true;
	if (AUmbraBaseCharacter* ControlledCharacter = Cast<AUmbraBaseCharacter>(GetCharacter()))
	{
		ControlledCharacter->Crouch();
	}
}

void AUmbraShadowController::OnStopCrouch()
{
	bWantsToJump = false;
	if (AUmbraBaseCharacter* ControlledCharacter = Cast<AUmbraBaseCharacter>(GetCharacter()))
	{
		ControlledCharacter->UnCrouch();
	}
}

void AUmbraShadowController::OnStartThrough()
{
	
}

void AUmbraShadowController::OnStopThrough()
{
	
}

void AUmbraShadowController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Green, InputTag.ToString());
}

void AUmbraShadowController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetAbilitySystemComponent() == nullptr) return;
	GetAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
}

void AUmbraShadowController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetAbilitySystemComponent() == nullptr) return;
	GetAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
}