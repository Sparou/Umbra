// Copyrighted by Vorona Games


#include "Player/UmbraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Character/Data/PlayerCharacterInfo.h"
#include "Kismet/GameplayStatics.h"

void AUmbraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(InputContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Subsystem as found"));
		Subsystem->AddMappingContext(InputContext, 0);
	}
}

void AUmbraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent) UE_LOG(LogTemp, Warning, TEXT("Enhanced Input Compoent was set!"));
	//EnhancedInputComponent->BindAction(SwitchCharacterAction, ETriggerEvent::Started, this, &AUmbraPlayerController::SwitchCharacter);
}

void AUmbraPlayerController::SwitchCharacter(FGameplayTag CharacterTag)
{
	TSubclassOf<AUmbraPlayerCharacter> NewCharacterBlueprint = PlayerCharactersInfo->FindPlayerCharacterBlueprintByTag(CharacterTag);
	if (OwnedCharacters.Contains(NewCharacterBlueprint))
	{
		AActor* NewCharacter = UGameplayStatics::GetActorOfClass(GetWorld(), NewCharacterBlueprint);
		Possess(Cast<APawn>(NewCharacter));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Character [%s] not found in OwnedCharacter of [%s]"),
			*CharacterTag.ToString(), *GetNameSafe(this));
	}
}


