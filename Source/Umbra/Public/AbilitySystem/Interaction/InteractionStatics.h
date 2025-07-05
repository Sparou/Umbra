// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InteractionStatics.generated.h"

class IInteractionInterface;
/**
 * 
 */
UCLASS()
class UMBRA_API UInteractionStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:

	//static void AppendInteractablesFromOverlapResult(const TArray<FOverlapResult>& OverlapResults, TArray<TScriptInterface<IInteractionInterface>>& OutInteractables);
};
