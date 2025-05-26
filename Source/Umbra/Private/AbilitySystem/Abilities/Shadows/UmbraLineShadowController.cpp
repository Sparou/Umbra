// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Shadows/UmbraLineShadowController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "UmbraGameplayTags.h"
#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Character/Data/PlayerCharacterInfo.h"
#include "GameFramework/CharacterMovementComponent.h"


void AUmbraLineShadowController::UpdateCloneOffset()
{
	if (!MasterCharacter || !GetPawn()) return;

	const float MoveThreshold = 50.f; // минимальное расстояние, при котором начинаем движение

	FVector RightVector = MasterCharacter->GetActorRightVector();
	FVector TargetLocation = MasterCharacter->GetActorLocation() + RightVector * FixedOffset;

	FVector MyLocation = GetPawn()->GetActorLocation();
	FVector ToTarget = TargetLocation - MyLocation;

	if (ToTarget.SizeSquared() > MoveThreshold * MoveThreshold) // быстрее чем Size()
	{
		FVector Direction = ToTarget.GetSafeNormal();
		GetPawn()->AddMovementInput(Direction, 1.0f);
	}
}

void AUmbraLineShadowController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCloneOffset();
}



UUmbraAbilitySystemComponent* AUmbraLineShadowController::GetAbilitySystemComponent()
{
	if (AbilitySystemComponent == nullptr)
	{
		AbilitySystemComponent = Cast<UUmbraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}

	return AbilitySystemComponent;
}

void AUmbraLineShadowController::Move(const FVector ForwardDirection, const float Y, const FVector RightDirection, const float X)
{
	//UE_LOG(LogTemp, Warning, TEXT("AUmbraShadowPlayerController::Move: %s"), *InputActionValue.ToString());
	

	UpdateCloneOffset();
}

void AUmbraLineShadowController::Look(const float X, const float Y)
{
	UpdateCloneOffset();
}

void AUmbraLineShadowController::OnStartWalking()
{
	UE_LOG(LogTemp, Error, TEXT("AUmbraShadowPlayerController::OnStartWalking"));
	bWantsToWalk = true;
	if (AUmbraBaseCharacter* ControlledCharacter = Cast<AUmbraBaseCharacter>(GetCharacter()))
	{
		ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = ControlledCharacter->GetMoveSpeed(FUmbraGameplayTags::Get().State_Stance_Standing, FUmbraGameplayTags::Get().State_Locomotion_Running);
	}
}

void AUmbraLineShadowController::OnStopWalking()
{
	bWantsToWalk = false;
	if (AUmbraBaseCharacter* ControlledCharacter = Cast<AUmbraBaseCharacter>(GetCharacter()))
	{
		ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = ControlledCharacter->GetMoveSpeed(FUmbraGameplayTags::Get().State_Stance_Standing, FUmbraGameplayTags::Get().State_Locomotion_Running);
	}
}

void AUmbraLineShadowController::OnStartJumping()
{
	bWantsToJump = true;
	if (AUmbraBaseCharacter* ControlledCharacter = Cast<AUmbraBaseCharacter>(GetCharacter()))
	{
		ControlledCharacter->Jump();
	}
}

void AUmbraLineShadowController::OnStopJumping()
{
	bWantsToJump = false;
}

void AUmbraLineShadowController::OnStartCrouch()
{
	bWantsToCrouch = true;
	if (AUmbraBaseCharacter* ControlledCharacter = Cast<AUmbraBaseCharacter>(GetCharacter()))
	{
		ControlledCharacter->Crouch();
	}
}

void AUmbraLineShadowController::OnStopCrouch()
{
	bWantsToJump = false;
	if (AUmbraBaseCharacter* ControlledCharacter = Cast<AUmbraBaseCharacter>(GetCharacter()))
	{
		ControlledCharacter->UnCrouch();
	}
}

void AUmbraLineShadowController::OnStartThrough()
{
	
}

void AUmbraLineShadowController::OnStopThrough()
{
	
}

void AUmbraLineShadowController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Green, InputTag.ToString());
}

void AUmbraLineShadowController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetAbilitySystemComponent() == nullptr) return;
	GetAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
}

void AUmbraLineShadowController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetAbilitySystemComponent() == nullptr) return;
	GetAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
}
