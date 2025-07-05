// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractionInterface.h"
#include "Interface/OutlineInterface.h"
#include "AbilitySystem/Interaction/InteractionOption.h"
#include "UmbraInteractableActor.generated.h"


UCLASS()
class UMBRA_API AUmbraInteractableActor : public AActor, public IInteractionInterface, public IOutlineInterface
{
	GENERATED_BODY()

public:
	
	AUmbraInteractableActor();

	FInteractionOption GetInteractionOption_Implementation() const;

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	FInteractionOption InteractionOption;

private:

	TArray<UMeshComponent*> Meshes;
};
