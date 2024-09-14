// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Character/UmbraBaseCharacter.h"
#include "Interaction/InteractionInterface.h"
#include "UmbraEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API AUmbraEnemyCharacter : public AUmbraBaseCharacter, public IInteractionInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void Interact(AActor* OtherActor) override;
};
