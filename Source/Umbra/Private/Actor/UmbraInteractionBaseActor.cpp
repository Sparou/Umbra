// Copyrighted by Vorona Games


#include "Actor/UmbraInteractionBaseActor.h"

AUmbraInteractionBaseActor::AUmbraInteractionBaseActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AUmbraInteractionBaseActor::EnableOutline_Implementation(int32 NewStencilValue)
{
	for (UMeshComponent* Mesh : Meshes)
	{
		Mesh->SetRenderCustomDepth(true);
		Mesh->SetCustomDepthStencilValue(NewStencilValue);
	}
}

void AUmbraInteractionBaseActor::DisableOutline_Implementation()
{
	for (UMeshComponent* Mesh : Meshes)
	{
		Mesh->SetRenderCustomDepth(false);
	}
}

void AUmbraInteractionBaseActor::BeginPlay()
{
	Super::BeginPlay();
	
	GetComponents<UMeshComponent>(Meshes);
}

void AUmbraInteractionBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


