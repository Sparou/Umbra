// Copyrighted by Vorona Games

#include "Game/UmbraGameModeBase.h"
#include "UmbraGameInstance.h"


APawn* AUmbraGameModeBase::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	UUmbraGameInstance* GI = Cast<UUmbraGameInstance>(GetGameInstance());
	if (GI && GI->SelectedPawnClass)
	{
		return GetWorld()->SpawnActor<APawn>(
			GI->SelectedPawnClass,
			StartSpot->GetActorLocation(),
			StartSpot->GetActorRotation()
		);
	}
	return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
}
