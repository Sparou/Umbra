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

void AUmbraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	check(InputContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(InputContext, 0);
	}
}

void AUmbraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UUmbraInputComponent* UmbraInputComponent = CastChecked<UUmbraInputComponent>(InputComponent);
	
	UmbraInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AUmbraPlayerController::Interact);
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
	IInteractionInterface* InteractableActor = Cast<IInteractionInterface>(InteractionComponent->OverlappedActor);

	if (InteractableActor)
	{
		InteractableActor->Interact(CurrentPlayerCharacter);
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



