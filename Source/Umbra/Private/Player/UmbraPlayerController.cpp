// Copyrighted by Vorona Games

#include "Player/UmbraPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Character/Component/TraversalComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/UmbraInputComponent.h"

void AUmbraPlayerController::SwitchToDefaultContext()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(InputContext, 0);
	}
}

void AUmbraPlayerController::SwitchToCameraOnlyContext()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(CameraOnlyInputContext, 0);
	}
}

void AUmbraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(InputContext);
	check(CameraOnlyInputContext);
	SwitchToDefaultContext();
}

void AUmbraPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (AbilitySystemComponent != nullptr)
	{
		AbilitySystemComponent->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void AUmbraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UUmbraInputComponent* UmbraInputComponent = CastChecked<UUmbraInputComponent>(InputComponent);
	
	UmbraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUmbraPlayerController::Move);
	UmbraInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AUmbraPlayerController::OnStopMoving);
	UmbraInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUmbraPlayerController::Look);
	UmbraInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &AUmbraPlayerController::CameraZoom);
	UmbraInputComponent->BindAction(ConfirmAction, ETriggerEvent::Started, this, &AUmbraPlayerController::ConfirmAbilityTargeting);
	UmbraInputComponent->BindAction(CancelAction, ETriggerEvent::Started, this, &AUmbraPlayerController::CancelAbilityTargeting);

	UmbraInputComponent->BindAbilityActions(InputConfig, this,
											&AUmbraPlayerController::AbilityInputTagPressed,
	                                        &AUmbraPlayerController::AbilityInputTagReleased,
	                                        &AUmbraPlayerController::AbilityInputTagHeld);
}

UUmbraAbilitySystemComponent* AUmbraPlayerController::GetAbilitySystemComponent()
{
	if (AbilitySystemComponent.IsValid())
	{
		return AbilitySystemComponent.Get();
	}

	if (GetPawn() == nullptr)
	{
		return nullptr;
	}
	
	if (UUmbraAbilitySystemComponent* ASC = Cast<UUmbraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>())))
	{
		AbilitySystemComponent = ASC;
		return ASC;
	}

	return nullptr;
}

UTraversalComponent* AUmbraPlayerController::GetTraversalComponent()
{
	if (TraversalComponent.IsValid())
	{
		return TraversalComponent.Get();
	}

	if (GetPawn() == nullptr)
	{
		return nullptr;
	}
	
	if (UTraversalComponent* TC = GetCharacter()->FindComponentByClass<UTraversalComponent>())
	{
		TraversalComponent = TC;
		return TC;
	}

	return nullptr;
}

UAnimInstance* AUmbraPlayerController::GetAnimInstance()
{
	if (AnimInstance.IsValid())
	{
		return AnimInstance.Get();
	}
	
	if (GetPawn() == nullptr)
	{
		return nullptr;
	}

	if (UAnimInstance* AI = GetCharacter()->GetMesh()->GetAnimInstance())
	{
		AnimInstance = AI;
	}

	return nullptr;
}

AUmbraBaseCharacter* AUmbraPlayerController::GetUmbraCharacter()
{
	if (UmbraCharacter.IsValid())
	{
		return UmbraCharacter.Get();
	}
	
	if (GetCharacter() == nullptr)
	{
		return nullptr;
	}

	if (AUmbraBaseCharacter* UC = Cast<AUmbraBaseCharacter>(GetCharacter()))
	{
		UmbraCharacter = UC;
		return UC;
	}

	return nullptr;
}

USpringArmComponent* AUmbraPlayerController::GetSpingArmComponent()
{
	if (SpringArmComponent.IsValid())
	{
		return SpringArmComponent.Get();
	}

	if (GetPawn() == nullptr)
	{
		return nullptr;
	}
	
	if (USpringArmComponent* SAC = GetPawn()->FindComponentByClass<USpringArmComponent>())
	{
		SpringArmComponent = SAC;
		return SAC;
	}

	return nullptr;
}

void AUmbraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (GetTraversalComponent())
	{
		TraversalComponent->AddMovementInput(InputAxisVector.Y, true);
		TraversalComponent->AddMovementInput(InputAxisVector.X, false);
	}
	else if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AUmbraPlayerController::Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
	APawn* CurrentPawn = GetPawn();
	CurrentPawn->AddControllerYawInput(LookAxisVector.X);
	CurrentPawn->AddControllerPitchInput(LookAxisVector.Y);
}

void AUmbraPlayerController::CameraZoom(const FInputActionValue& InputActionValue)
{
	if (GetSpingArmComponent())
	{
		SpringArmComponent->TargetArmLength = FMath::Clamp(
			SpringArmComponent->TargetArmLength - CameraZoomStep * InputActionValue.Get<float>(),
			MinCameraZoom,
			MaxCameraZoom);
	}
}

void AUmbraPlayerController::OnStopMoving()
{
	if (GetTraversalComponent())
	{
		if (HasAuthority())
		{
			TraversalComponent->ResetMovement();
		}
		else
		{
			TraversalComponent->ServerResetMovement();
		}
	}
}

void AUmbraPlayerController::OnStartDrop()
{
	if (GetTraversalComponent())
	{
		HasAuthority() ? TraversalComponent->DropFromClimb() : TraversalComponent->ServerDropFromClimb();
	}
}

void AUmbraPlayerController::ConfirmAbilityTargeting()
{
	if (GetAbilitySystemComponent())
	{
		AbilitySystemComponent->LocalInputConfirm();
	}
}

void AUmbraPlayerController::CancelAbilityTargeting()
{
	if (GetAbilitySystemComponent())
	{
		AbilitySystemComponent->LocalInputCancel();
	}
}

void AUmbraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetAbilitySystemComponent() == nullptr) return;
	GetAbilitySystemComponent()->AbilityInputTagPressed(InputTag);
}

void AUmbraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetAbilitySystemComponent() == nullptr) return;
	GetAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
}

void AUmbraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	
}
