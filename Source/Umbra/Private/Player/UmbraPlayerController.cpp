// Copyrighted by Vorona Games


#include "Player/UmbraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Character/UmbraEnemyCharacter.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Character/Component/InteractionComponent.h"
#include "Character/Data/PlayerCharacterInfo.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	
	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AUmbraPlayerController::Interact);

	EnhancedInputComponent->BindAction(SwitchToAssassinAction, ETriggerEvent::Started, this,
	                                   &AUmbraPlayerController::SwitchCharacter, FUmbraGameplayTags::Get().Character_Assassin);

	EnhancedInputComponent->BindAction(SwitchToTrapperAction, ETriggerEvent::Started, this,
		&AUmbraPlayerController::SwitchCharacter, FUmbraGameplayTags::Get().Character_Trapper);
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



