// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractionInterface.h"
#include "Interface/OutlineInterface.h"
#include "UmbraInteractionBaseActor.generated.h"

UCLASS()
class UMBRA_API AUmbraInteractionBaseActor : public AActor, public IInteractionInterface, public IOutlineInterface
{
	GENERATED_BODY()
	
public:
	
	AUmbraInteractionBaseActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Outline")
	int32 StencilValue;
	
	/* Outline Interface */
	virtual void EnableOutline_Implementation(int32 NewStencilValue) override;

	virtual void DisableOutline_Implementation() override;
	
private:

	UPROPERTY()
	TArray<UMeshComponent*> Meshes;
};
