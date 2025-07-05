// Copyrighted by Vorona Games


#include "Actor/UmbraInteractableActor.h"

AUmbraInteractableActor::AUmbraInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;
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


