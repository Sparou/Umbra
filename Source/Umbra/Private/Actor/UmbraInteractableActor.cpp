// Copyrighted by Vorona Games


#include "Actor/UmbraInteractableActor.h"

AUmbraInteractableActor::AUmbraInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AUmbraInteractableActor::GatherInteractionOption(const FInteractionQuery& Query,
	FInteractionOptionBuilder& Builder)
{
	Builder.AddInteractionOption(InteractionOption);
}

void AUmbraInteractableActor::CustomizeInteractionEventData(const FGameplayTag& EventTag,
	FGameplayEventData& InOutEventData)
{
	IInteractionInterface::CustomizeInteractionEventData(EventTag, InOutEventData);
}

void AUmbraInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
	GetComponents<UMeshComponent>(Meshes);

	for (UMeshComponent* Mesh : Meshes)
	{
		Mesh->SetRenderCustomDepth(true);
	}
}


