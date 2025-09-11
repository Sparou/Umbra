// Copyrighted by Vorona Games

#include "Character/Component/HeroComponent.h"
#include "EnhancedInputSubsystems.h"

DEFINE_LOG_CATEGORY(HeroComponentLog);

UHeroComponent::UHeroComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHeroComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	checkf(PlayerInputComponent, TEXT("Input Component must be a valid in InitializePlayerInput from [%s]"), *GetNameSafe(this));

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		UE_LOG(HeroComponentLog, Error, TEXT("Pawn is not valid in [%s]"), *GetNameSafe(this));
		return;
	}

	const APlayerController* PlayerController = GetController<APlayerController>();
	checkf(PlayerController, TEXT("Player Controller must be a valid in InitializePlayerInput from [%s]"), *GetNameSafe(this));

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	checkf(Subsystem, TEXT("Subsystem must be a valid in InitializePlayerInput from [%s]"), *GetNameSafe(this));

	
}
