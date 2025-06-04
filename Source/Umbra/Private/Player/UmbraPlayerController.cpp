// Copyrighted by Vorona Games

#include "Player/UmbraPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Character/Component/InteractionComponent.h"
#include "Character/Component/TagManager.h"
#include "TraversalSystem/TraversalComponent.h"
#include "Character/Data/PlayerCharacterInfo.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/UmbraInputComponent.h"
#include "Interface/InteractionInterface.h"
#include "Kismet/GameplayStatics.h"

void AUmbraPlayerController::SwitchToDefaultContext()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(InputContext, 0);
	}
}

void AUmbraPlayerController::SwitchToCameraOnlyContext()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(CameraOnlyInputContext, 0);
	}
}

void AUmbraPlayerController::SwitchToArrowContext()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "SwitchToArrowContext");
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(ArrowContext, 0);
	}
}


void AUmbraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(InputContext); 
	check(CameraOnlyInputContext);
	SwitchToDefaultContext();
}

void AUmbraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UUmbraInputComponent* UmbraInputComponent = CastChecked<UUmbraInputComponent>(InputComponent);
	
	UmbraInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AUmbraPlayerController::OnInteract);
	UmbraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUmbraPlayerController::Move);
	UmbraInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &AUmbraPlayerController::OnStartMoving);
	UmbraInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AUmbraPlayerController::OnStopMoving);
	UmbraInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUmbraPlayerController::Look);
	UmbraInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AUmbraPlayerController::OnStartJumping);
	UmbraInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AUmbraPlayerController::OnStopJumping);
	UmbraInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &AUmbraPlayerController::OnStartWalking);
	UmbraInputComponent->BindAction(WalkAction, ETriggerEvent::Completed, this, &AUmbraPlayerController::OnStopWalking);
	UmbraInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AUmbraPlayerController::OnStartCrouch);
	UmbraInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AUmbraPlayerController::OnStopCrouch);
	UmbraInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AUmbraPlayerController::OnStartThrough);
	UmbraInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AUmbraPlayerController::OnStopThrough);
	UmbraInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &AUmbraPlayerController::CameraZoom);

	// For arrow movement
	UmbraInputComponent->BindAction(ArrowAction, ETriggerEvent::Triggered, this, &AUmbraPlayerController::DirectArrow);
	UmbraInputComponent->BindAction(DropAction, ETriggerEvent::Started, this, &AUmbraPlayerController::OnStartDrop);
	
	UmbraInputComponent->BindAbilityActions(InputConfig, this, &AUmbraPlayerController::AbilityInputTagPressed,
	                                        &AUmbraPlayerController::AbilityInputTagReleased, &AUmbraPlayerController::AbilityInputTagHeld);
}

UUmbraAbilitySystemComponent* AUmbraPlayerController::GetAbilitySystemComponent()
{
	if (AbilitySystemComponent == nullptr)
	{
		AbilitySystemComponent = Cast<UUmbraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}

	return AbilitySystemComponent;
}

UTraversalComponent* AUmbraPlayerController::GetTraversalComponent()
{
	if (TraversalComponent == nullptr)
	{
		TraversalComponent = GetCharacter()->GetComponentByClass<UTraversalComponent>();
	}

	return TraversalComponent;
}

UAnimInstance* AUmbraPlayerController::GetAnimInstance()
{
	if (AnimInstance == nullptr)
	{
		AnimInstance = GetCharacter()->GetMesh()->GetAnimInstance();
	}

	return AnimInstance;
}

UTagManager* AUmbraPlayerController::GetTagManager()
{
	if (TagManager == nullptr)
	{
		TagManager = GetCharacter()->GetComponentByClass<UTagManager>();
	}

	return TagManager;
}

AUmbraBaseCharacter* AUmbraPlayerController::GetControlledCharacter()
{
	if (!ControlledCharacter)
	{
		ControlledCharacter = Cast<AUmbraBaseCharacter>(GetCharacter());
	}

	return ControlledCharacter;
}

UInteractionComponent* AUmbraPlayerController::GetInteractionComponent()
{
	if (!InteractionComponent)
	{
		InteractionComponent = GetCharacter()->GetComponentByClass<UInteractionComponent>();
	}

	return InteractionComponent;
}

USpringArmComponent* AUmbraPlayerController::GetSpingArmComponent()
{
	if (!SpringArmComponent)
	{
		SpringArmComponent = GetCharacter()->GetComponentByClass<USpringArmComponent>();
	}

	return SpringArmComponent;
}

void AUmbraPlayerController::OnInteract()
{
	if (!GetInteractionComponent())
	{
		return;
	}
	
	if (IsLocalController())
	{
		ServerInteract(InteractionComponent->GetInteractionActor());
	}
	else
	{
		Interact(InteractionComponent->GetInteractionActor());
	}
}

void AUmbraPlayerController::Interact(AActor* InteractionTarget)
{
	if (InteractionTarget && InteractionTarget->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		IInteractionInterface::Execute_Interact(InteractionTarget, GetCharacter(), true);
	}
}

void AUmbraPlayerController::ServerInteract_Implementation(AActor* InteractionTarget)
{
	Interact(InteractionTarget);
}

void AUmbraPlayerController::DirectArrow(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		if (UProjectileMovementComponent* MoveComp = ControlledPawn->FindComponentByClass<UProjectileMovementComponent>())
		{
			const FVector CurrentForward = MoveComp->Velocity.GetSafeNormal();
			if (CurrentForward.IsZero())
			{
				return;
			}

			const FVector WorldUp = FVector::UpVector;
			const FVector RightVector = FVector::CrossProduct(WorldUp, CurrentForward).GetSafeNormal();
			

			FVector DesiredDirection = CurrentForward 
							+ RightVector * InputAxisVector.X 
							+ InputAxisVector.Y;


			DesiredDirection = DesiredDirection.GetSafeNormal();

			const float RotationSpeed = 2.0f;
        
			if (!DesiredDirection.IsZero())
			{
				FQuat CurrentQuat = ControlledPawn->GetActorQuat();

				// Поворот по локальной оси Y (Pitch)
				FQuat PitchQuat = FQuat(ControlledPawn->GetActorRightVector(), FMath::DegreesToRadians(InputAxisVector.Y * RotationSpeed));

				// Поворот по локальной оси Z (Yaw)
				FQuat YawQuat = FQuat(FVector::UpVector, FMath::DegreesToRadians(InputAxisVector.X * RotationSpeed));

				FQuat NewQuat = YawQuat * PitchQuat * CurrentQuat;
				ControlledPawn->SetActorRotation(NewQuat);

				// Получаем новое направление движения после поворота
				FVector NewForward = ControlledPawn->GetActorForwardVector();
				MoveComp->Velocity = NewForward * MoveComp->InitialSpeed; // или другая желаемая скорость
			}
		}
	}
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
	else if (APawn* ControlledPawn = GetPawn<APawn>()) {
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

void AUmbraPlayerController::OnStartMoving()
{
	GetTagManager()->AddTag(FUmbraGameplayTags::Get().State_Movement_Moving);
}

void AUmbraPlayerController::OnStopMoving()
{
	GetTagManager()->RemoveTag(FUmbraGameplayTags::Get().State_Movement_Moving);

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

void AUmbraPlayerController::OnStartWalking()
{
	SetWalking(true);

	if (!HasAuthority())
	{
		ServerSetWalking(true);
	}
}

void AUmbraPlayerController::OnStopWalking()
{
	SetWalking(false);

	if (!HasAuthority())
	{
		ServerSetWalking(false);
	}
}

void AUmbraPlayerController::OnStartJumping()
{
	if (GetControlledCharacter() && GetTraversalComponent())
	{
		if (HasAuthority())
		{
			TraversalComponent->TriggerTraversalAction(true);
		}
		else
		{
			TraversalComponent->ServerTriggerTraversalAction(true);
		}
	}
}

void AUmbraPlayerController::OnStopJumping()
{
	bWantsToJump = false;
}

void AUmbraPlayerController::OnStartCrouch()
{
	if (GetControlledCharacter())
	{
		if (ControlledCharacter->GetCharacterMovement()->IsFalling())
		{
			return;
		}
		ControlledCharacter->Crouch();
		GetTagManager()->AddTag(FUmbraGameplayTags::Get().State_Stance_Crouching);
	}
}

void AUmbraPlayerController::OnStopCrouch()
{
	if (GetControlledCharacter())
	{
		ControlledCharacter->UnCrouch();
		GetTagManager()->RemoveTag(FUmbraGameplayTags::Get().State_Stance_Crouching);
	}
}

void AUmbraPlayerController::OnStartDrop()
{
	if (GetTraversalComponent())
	{
		HasAuthority() ? TraversalComponent->DropFromClimb() : TraversalComponent->ServerDropFromClimb();
	}
}

void AUmbraPlayerController::OnStartThrough()
{
	
}

void AUmbraPlayerController::OnStopThrough()
{
	
}

void AUmbraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Green, InputTag.ToString());
}

void AUmbraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetAbilitySystemComponent() == nullptr) return;
	GetAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
}

void AUmbraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetAbilitySystemComponent() == nullptr) return;
	GetAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
}

void AUmbraPlayerController::SetWalking(bool bWalking)
{
	if (!GetControlledCharacter()) return;
	
	if (bWalking)
	{
		if (ControlledCharacter->GetCharacterMovement()->IsCrouching())
		{
			ControlledCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = ControlledCharacter->GetMoveSpeed(
				FUmbraGameplayTags::Get().State_Stance_Crouching,
				FUmbraGameplayTags::Get().State_Locomotion_Walking);
		}
		else
		{
			ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = ControlledCharacter->GetMoveSpeed(
				FUmbraGameplayTags::Get().State_Stance_Standing,
				FUmbraGameplayTags::Get().State_Locomotion_Walking);
		}
		GetTagManager()->AddTag(FUmbraGameplayTags::Get().State_Locomotion_Walking);
	}
	else
	{
		if (ControlledCharacter->GetCharacterMovement()->IsCrouching())
		{
			ControlledCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = ControlledCharacter->GetMoveSpeed(
				FUmbraGameplayTags::Get().State_Stance_Crouching,
				FUmbraGameplayTags::Get().State_Locomotion_Running);
		}
		else
		{
			ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = ControlledCharacter->GetMoveSpeed(
				FUmbraGameplayTags::Get().State_Stance_Standing,
				FUmbraGameplayTags::Get().State_Locomotion_Running);
		}
		GetTagManager()->RemoveTag(FUmbraGameplayTags::Get().State_Locomotion_Walking);
	}
}

void AUmbraPlayerController::ServerSetWalking_Implementation(bool bWalking)
{
	SetWalking(bWalking);
}



