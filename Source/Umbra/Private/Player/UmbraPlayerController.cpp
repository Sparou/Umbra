// Copyrighted by Vorona Games


#include "Player/UmbraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "Character/UmbraEnemyCharacter.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Character/Component/InteractionComponent.h"
#include "Character/Data/PlayerCharacterInfo.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Input/UmbraInputComponent.h"
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
}

void AUmbraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UUmbraInputComponent* UmbraInputComponent = CastChecked<UUmbraInputComponent>(InputComponent);
	
	UmbraInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AUmbraPlayerController::Interact);
	UmbraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUmbraPlayerController::Move);
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

	if (APawn* ControlledPawn = GetPawn<APawn>()) {
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

void AUmbraPlayerController::OnStartWalking()
{
	bWantsToWalk = true;
	if (AUmbraBaseCharacter* ControlledCharacter = Cast<AUmbraBaseCharacter>(GetCharacter()))
	{
		ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = ControlledCharacter->BaseWalkSpeed;
	}
}

void AUmbraPlayerController::OnStopWalking()
{
	bWantsToWalk = false;
	if (AUmbraBaseCharacter* ControlledCharacter = Cast<AUmbraBaseCharacter>(GetCharacter()))
	{
		ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = ControlledCharacter->BaseRunSpeed;
	}
}

void AUmbraPlayerController::OnStartJumping()
{
	bWantsToJump = true;
	if (AUmbraBaseCharacter* ControlledCharacter = Cast<AUmbraBaseCharacter>(GetCharacter()))
	{
		ControlledCharacter->Jump();
	}
}

void AUmbraPlayerController::OnStopJumping()
{
	bWantsToJump = false;
}

void AUmbraPlayerController::OnStartCrouch()
{
	bWantsToCrouch = true;
	if (AUmbraBaseCharacter* ControlledCharacter = Cast<AUmbraBaseCharacter>(GetCharacter()))
	{
		ControlledCharacter->Crouch();
	}
}

void AUmbraPlayerController::OnStopCrouch()
{
	bWantsToJump = false;
	if (AUmbraBaseCharacter* ControlledCharacter = Cast<AUmbraBaseCharacter>(GetCharacter()))
	{
		ControlledCharacter->UnCrouch();
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



