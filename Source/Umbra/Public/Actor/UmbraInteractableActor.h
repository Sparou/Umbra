// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractionInterface.h"
#include "Interface/OutlineInterface.h"
#include "UmbraInteractableActor.generated.h"


USTRUCT()
struct FInteractionOption
{
	GENERATED_BODY()
	
	FString Tooltip = FString("Interact");
	
	FGameplayAbilitySpecHandle InteractionAbility;
};

UCLASS()
class UMBRA_API AUmbraInteractableActor : public AActor, public IInteractionInterface, public IOutlineInterface
{
	GENERATED_BODY()

public:
	
	AUmbraInteractableActor();

protected:
	
	virtual void BeginPlay() override;

private:

	TArray<UMeshComponent*> Meshes;
};
