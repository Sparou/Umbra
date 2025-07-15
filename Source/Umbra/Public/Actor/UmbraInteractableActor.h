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

	virtual void GatherInteractionOption(const FInteractionQuery& Query, FInteractionOptionBuilder& Builder) override;
	virtual void CustomizeInteractionEventData(const FGameplayTag& EventTag, FGameplayEventData& InOutEventData) override;

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FInteractionOption InteractionOption;

private:

	TArray<UMeshComponent*> Meshes;
};
