// Copyrighted by Vorona Games


#include "Player/UmbraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Character/UmbraPlayerCharacter.h"
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
	EnhancedInputComponent->BindAction(SwitchCharacterAction, ETriggerEvent::Started, this, &AUmbraPlayerController::SwitchCharacter, );
}

void AUmbraPlayerController::SwitchCharacter()
{
	if 
}


