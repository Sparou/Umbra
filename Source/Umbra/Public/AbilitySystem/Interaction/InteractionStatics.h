// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Interface/InteractionInterface.h"
#include "InteractionStatics.generated.h"

class AUmbraInteractableActor;
template <typename InterfaceType> class TScriptInterface;

/**
 * 
 */
UCLASS()
class UMBRA_API UInteractionStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static AActor* GetActorFromInteractableInterface(TScriptInterface<IInteractionInterface> Interactable);
	static void AppendInteractablesFromOverlapResult(const TArray<FOverlapResult>& OverlapResults, TArray<TScriptInterface<IInteractionInterface>>& OutInteractables);
	static void AppendInteractablesFromHitResult(const FHitResult& HitResult, TArray<TScriptInterface<IInteractionInterface>>& OutInteractables);
};
