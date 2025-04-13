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
#include "Input/UmbraInputComponent.h"
#include "Interface/UmbraAnimationInterface.h"
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

void AUmbraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(InputContext); 
	check(CameraOnlyInputContext);
	SwitchToDefaultContext();

	TraversalComponent = GetPawn()->FindComponentByClass<UTraversalComponent>();
	check(TraversalComponent);
}

void AUmbraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UUmbraInputComponent* UmbraInputComponent = CastChecked<UUmbraInputComponent>(InputComponent);
	
	UmbraInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AUmbraPlayerController::Interact);
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
	
	UmbraInputComponent->BindAbilityActions(InputConfig, this, &AUmbraPlayerController::AbilityInputTagPressed,
	                                        &AUmbraPlayerController::AbilityInputTagReleased, &AUmbraPlayerController::AbilityInputTagHeld);

	UmbraInputComponent->BindAction(SwitchToAssassinAction, ETriggerEvent::Started, this,
	                                   &AUmbraPlayerController::SwitchCharacter, FUmbraGameplayTags::Get().Character_Assassin);

	UmbraInputComponent->BindAction(SwitchToTrapperAction, ETriggerEvent::Started, this,
		&AUmbraPlayerController::SwitchCharacter, FUmbraGameplayTags::Get().Character_Trapper);
}



UUmbraAbilitySystemComponent* AUmbraPlayerController::GetAbilitySystemComponent()
{
	if (AbilitySystemComponent == nullptr)
	{
		AbilitySystemComponent = Cast<UUmbraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}

	return AbilitySystemComponent;
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


void AUmbraPlayerController::SwitchCharacter(FGameplayTag CharacterTag)
{
	if (OwnedCharacters.Num() == 0) return;

	//TODO: Добавить защиту от переключение на уже используемого персонажа
	
	TSubclassOf<AUmbraPlayerCharacter> NewCharacterBlueprint = PlayerCharactersInfo->FindPlayerCharacterBlueprintByTag(CharacterTag);
	if (IsValid(NewCharacterBlueprint) && OwnedCharacters.Contains(NewCharacterBlueprint))
	{
		if (AActor* NewCharacter = UGameplayStatics::GetActorOfClass(GetWorld(), NewCharacterBlueprint))
		{
			Possess(Cast<APawn>(NewCharacter));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Character [%s] not found in OwnedCharacters of [%s]"),
			*CharacterTag.ToString(), *GetNameSafe(this));
	}
}

void AUmbraPlayerController::Interact()
{
	APawn* CurrentPawn = GetPawn();
	AUmbraPlayerCharacter* CurrentPlayerCharacter = Cast<AUmbraPlayerCharacter>(CurrentPawn);
	UInteractionComponent* InteractionComponent = CurrentPlayerCharacter->GetComponentByClass<UInteractionComponent>();
	AActor* Target = InteractionComponent->GetCurrentTarget();
	UE_LOG(LogTemp, Warning, TEXT("Target: %s"), *Target->GetName());
}

void AUmbraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (TraversalComponent)
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
	CurrentPawn->AddControllerPitchInput(-LookAxisVector.Y);
}

void AUmbraPlayerController::OnStartMoving()
{
	GetTagManager()->AddTag(FUmbraGameplayTags::Get().State_Movement_Moving);
}

void AUmbraPlayerController::OnStopMoving()
{
	GetTagManager()->RemoveTag(FUmbraGameplayTags::Get().State_Movement_Moving);
	if (TraversalComponent)
	{
		TraversalComponent->ResetMovement();
	}
}

void AUmbraPlayerController::OnStartWalking()
{
	if (GetControlledCharacter())
	{
		if (ControlledCharacter->GetCharacterMovement()->IsCrouching())
		{
			ControlledCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = ControlledCharacter->GetMoveSpeed(FUmbraGameplayTags::Get().State_Stance_Crouching, FUmbraGameplayTags::Get().State_Locomotion_Walking);
		}
		else
		{
			ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = ControlledCharacter->GetMoveSpeed(FUmbraGameplayTags::Get().State_Stance_Standing, FUmbraGameplayTags::Get().State_Locomotion_Walking);
		}
		GetTagManager()->AddTag(FUmbraGameplayTags::Get().State_Locomotion_Walking);
	}
}

void AUmbraPlayerController::OnStopWalking()
{
	if (GetControlledCharacter())
	{
		if (ControlledCharacter->GetCharacterMovement()->IsCrouching())
		{
			ControlledCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = ControlledCharacter->GetMoveSpeed(FUmbraGameplayTags::Get().State_Stance_Crouching, FUmbraGameplayTags::Get().State_Locomotion_Running);
		}
		else
		{
			ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = ControlledCharacter->GetMoveSpeed(FUmbraGameplayTags::Get().State_Stance_Standing, FUmbraGameplayTags::Get().State_Locomotion_Running);
		}
		GetTagManager()->RemoveTag(FUmbraGameplayTags::Get().State_Locomotion_Walking);
	}
}

void AUmbraPlayerController::OnStartJumping()
{
	if (GetControlledCharacter() && TraversalComponent)
	{
		GetTagManager()->AddTag(FUmbraGameplayTags::Get().State_Movement_Falling);
		TraversalComponent->TriggerTraversalAction(true);
	}
	
	// if (GetControlledCharacter())
	// {
	// 	ControlledCharacter->Jump();
	// 	GetTagManager()->AddTag(FUmbraGameplayTags::Get().State_Movement_Falling);
	// }
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



