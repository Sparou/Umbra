// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UmbraGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TSubclassOf<APawn> GetPawn();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<APawn> SelectedPawnClass;
};
