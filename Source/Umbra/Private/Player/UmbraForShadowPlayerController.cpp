// Copyrighted by Vorona Games


#include "Player/UmbraForShadowPlayerController.h"

#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "AbilitySystem/Abilities/Shadows/UmbraLineShadowController.h"
#include "AbilitySystem/Abilities/Shadows/UmbraShadowController.h"
#include "Input/UmbraInputComponent.h"

// void AUmbraForShadowPlayerController::SetupInputComponent()
// {
// 	Super::SetupInputComponent();
//
// 	UUmbraInputComponent* UmbraInputComponent = CastChecked<UUmbraInputComponent>(InputComponent);
// 	UmbraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUmbraForShadowPlayerController::Move);
// }
//
// void AUmbraForShadowPlayerController::Move(const FInputActionValue& InputActionValue)
// {
// 	Super::Move(InputActionValue);
// 	
// 	UE_LOG(LogTemp, Warning, TEXT("AUmbraForShadowPlayerController::Move"));
//
// 	
// 	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
// 	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
//
// 	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
// 	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
// 	
// 	if (ShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraShadowController* ShadowController : ShadowControllers)
// 		{
// 			ShadowController->Move(ForwardDirection, InputAxisVector.Y, RightDirection, InputAxisVector.X);
// 		}
// 	}
// 	if (LineShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraLineShadowController* ShadowController : LineShadowControllers)
// 		{
// 			ShadowController->Move(ForwardDirection, InputAxisVector.Y, RightDirection, InputAxisVector.X);
// 		}
// 	}
// }
//
// void AUmbraForShadowPlayerController::Look(const FInputActionValue& InputActionValue)
// {
// 	Super::Look(InputActionValue);
// 	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
// 	if (ShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraShadowController* ShadowController : ShadowControllers)
// 		{
// 			ShadowController->Look(LookAxisVector.X, -LookAxisVector.Y);
// 		}
// 	}
// 	if (LineShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraLineShadowController* ShadowController : LineShadowControllers)
// 		{
// 			ShadowController->Look(LookAxisVector.X, -LookAxisVector.Y);
// 		}
// 	}
// }
//
// void AUmbraForShadowPlayerController::OnStartWalking()
// {
// 	Super::OnStartWalking();
// 	if (ShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraShadowController* ShadowController : ShadowControllers)
// 		{
// 			ShadowController->OnStartWalking();
// 		}
// 	}
// 	if (LineShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraLineShadowController* ShadowController : LineShadowControllers)
// 		{
// 			ShadowController->OnStartWalking();
// 		}
// 	}
// }
//
// void AUmbraForShadowPlayerController::OnStopWalking()
// {
// 	Super::OnStopWalking();
// 	if (ShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraShadowController* ShadowController : ShadowControllers)
// 		{
// 			ShadowController->OnStopWalking();
// 		}
// 	}
// 	if (LineShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraLineShadowController* ShadowController : LineShadowControllers)
// 		{
// 			ShadowController->OnStopWalking();
// 		}
// 	}
// }
//
// void AUmbraForShadowPlayerController::OnStartJumping()
// {
// 	Super::OnStartJumping();
// 	if (ShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraShadowController* ShadowController : ShadowControllers)
// 		{
// 			ShadowController->OnStartJumping();
// 		}
// 	}
// 	if (LineShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraLineShadowController* ShadowController : LineShadowControllers)
// 		{
// 			ShadowController->OnStartJumping();
// 		}
// 	}
// }
//
// void AUmbraForShadowPlayerController::OnStopJumping()
// {
// 	Super::OnStopJumping();
// 	if (ShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraShadowController* ShadowController : ShadowControllers)
// 		{
// 			ShadowController->OnStopJumping();
// 		}
// 	}
// 	if (LineShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraLineShadowController* ShadowController : LineShadowControllers)
// 		{
// 			ShadowController->OnStopJumping();
// 		}
// 	}
// }
//
// void AUmbraForShadowPlayerController::OnStartCrouch()
// {
// 	Super::OnStartCrouch();
// 	if (ShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraShadowController* ShadowController : ShadowControllers)
// 		{
// 			ShadowController->OnStartCrouch();
// 		}
// 	}
// 	if (LineShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraLineShadowController* ShadowController : LineShadowControllers)
// 		{
// 			ShadowController->OnStartCrouch();
// 		}
// 	}
// }
//
// void AUmbraForShadowPlayerController::OnStopCrouch()
// {
// 	Super::OnStopCrouch();
// 	if (ShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraShadowController* ShadowController : ShadowControllers)
// 		{
// 			ShadowController->OnStopCrouch();
// 		}
// 	}
// 	if (LineShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraLineShadowController* ShadowController : LineShadowControllers)
// 		{
// 			ShadowController->OnStopCrouch();
// 		}
// 	}
// }
//
// void AUmbraForShadowPlayerController::OnStartThrough()
// {
// 	Super::OnStartThrough();
// }
//
// void AUmbraForShadowPlayerController::OnStopThrough()
// {
// 	Super::OnStopThrough();
// }
//
// void AUmbraForShadowPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
// {
// 	Super::AbilityInputTagPressed(InputTag);
// }
//
// void AUmbraForShadowPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
// {
// 	Super::AbilityInputTagReleased(InputTag);
// 	if (ShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraShadowController* ShadowController : ShadowControllers)
// 		{
// 			ShadowController->AbilityInputTagReleased(InputTag);
// 		}
// 	}
// 	if (LineShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraLineShadowController* ShadowController : LineShadowControllers)
// 		{
// 			ShadowController->AbilityInputTagReleased(InputTag);
// 		}
// 	}
// }
//
// void AUmbraForShadowPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
// {
// 	Super::AbilityInputTagHeld(InputTag);
// 	if (ShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraShadowController* ShadowController : ShadowControllers)
// 		{
// 			ShadowController->AbilityInputTagHeld(InputTag);
// 		}
// 	}
// 	if (LineShadowControllers.Num() > 0)
// 	{
// 		for (AUmbraLineShadowController* ShadowController : LineShadowControllers)
// 		{
// 			ShadowController->AbilityInputTagHeld(InputTag);
// 		}
// 	}
// }
