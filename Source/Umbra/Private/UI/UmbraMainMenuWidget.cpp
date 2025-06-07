// Copyrighted by Vorona Games


#include "UI/UmbraMainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "UmbraGameInstance.h"
#include "Engine/World.h"


void UUmbraMainMenuWidget::UpdateSelection(int32 Direction)
{
	UE_LOG(LogTemp, Warning, TEXT("UpdateSelection"));
	if (SpawnedActor)
	{
		SpawnedActor->Destroy();
		SpawnedActor = nullptr;
	}

	int32 NumActors = MenuActors.Num();
	if (NumActors == 0) return;

	CurrentIndex = (CurrentIndex + Direction + NumActors) % NumActors;

	SpawnCurrentActor();
}

void UUmbraMainMenuWidget::SpawnCurrentActor()
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnCurrentActor CurrentIndex: %d"), CurrentIndex);
	
	if (!MenuActors.IsValidIndex(CurrentIndex)) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnedActor = GetWorld()->SpawnActor<AActor>(MenuActors[CurrentIndex], SpawnLocation, SpawnRotation, SpawnParams);
}

void UUmbraMainMenuWidget::SaveChoose()
{
	UUmbraGameInstance* MenuGI = Cast<UUmbraGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (MenuGI && CorrespondingPawns.IsValidIndex(CurrentIndex))
	{
		MenuGI->SelectedPawnClass = CorrespondingPawns[CurrentIndex];
	}
	UE_LOG(LogTemp, Warning, TEXT("Saved Pawn Class: %s"), *MenuGI->SelectedPawnClass->GetName());
}