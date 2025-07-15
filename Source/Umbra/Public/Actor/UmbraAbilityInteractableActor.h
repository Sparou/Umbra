// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UmbraInteractableActor.h"
#include "UmbraAbilityInteractableActor.generated.h"

UCLASS()
class UMBRA_API AUmbraAbilityInteractableActor : public AUmbraInteractableActor
{
	GENERATED_BODY()

public:
	AUmbraAbilityInteractableActor();

protected:

	virtual void BeginPlay() override;

	// ~ Start of Interaction Interface
	virtual void GatherInteractionOption(const FInteractionQuery& Query, FInteractionOptionBuilder& Builder) override;
	// ~ End of Interaction Interface
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction")
	TSubclassOf<UGameplayAbility> InteractionAbility;

private:
	void AddInteractableAbilities();
};
