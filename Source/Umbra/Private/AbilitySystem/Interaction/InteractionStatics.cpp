// Copyrighted by Vorona Games


#include "AbilitySystem/Interaction/InteractionStatics.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Actor/UmbraInteractableActor.h"
#include "Engine/OverlapResult.h"

AActor* UInteractionStatics::GetActorFromInteractableInterface(TScriptInterface<IInteractionInterface> Interactable)
{
	if (UObject* Object = Interactable.GetObject())
	{
		if (AActor* Actor = Cast<AActor>(Object))
		{
			return Actor;
		}
		else if (UActorComponent* Component = Cast<UActorComponent>(Object))
		{
			return Component->GetOwner();
		}
		else
		{
			unimplemented();
		}
	}
	return nullptr;
}

void UInteractionStatics::AppendInteractablesFromOverlapResult(const TArray<FOverlapResult>& OverlapResults, TArray<TScriptInterface<IInteractionInterface>>& OutInteractables)
{
	for (const FOverlapResult& Overlap : OverlapResults)
	{
		TScriptInterface<IInteractionInterface> InteractableActor(Overlap.GetActor());
		if (InteractableActor)
		{
			OutInteractables.AddUnique(InteractableActor);
		}

		TScriptInterface<IInteractionInterface> InteractableComponent(Overlap.GetComponent());
		if (InteractableComponent)
		{
			OutInteractables.AddUnique(InteractableComponent);
		}
	}
}

void UInteractionStatics::AppendInteractablesFromHitResult(const FHitResult& HitResult, TArray<TScriptInterface<IInteractionInterface>>& OutInteractables)
{
	TScriptInterface<IInteractionInterface> InteractableActor(HitResult.GetActor());
	if (InteractableActor)
	{
		OutInteractables.AddUnique(InteractableActor);
	}

	TScriptInterface<IInteractionInterface> InteractableComponent(HitResult.GetComponent());
	if (InteractableComponent)
	{
		OutInteractables.AddUnique(InteractableComponent);
	}
}
