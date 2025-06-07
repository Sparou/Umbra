// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UmbraGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API AUmbraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;
};
