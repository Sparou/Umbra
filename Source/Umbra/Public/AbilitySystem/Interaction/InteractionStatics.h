// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Interface/InteractionInterface.h"
#include "InteractionStatics.generated.h"

template <typename InterfaceType> class TScriptInterface;

/**
 * 
 */
UCLASS()
class UMBRA_API UInteractionStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static void AppendInteractablesFromOverlapResult(const TArray<FOverlapResult>& OverlapResults, TArray<TScriptInterface<IInteractionInterface>>& OutInteractables);
};
