// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Character/UmbraBaseCharacter.h"
#include "Interaction/InteractionInterface.h"
#include "Interface/PatrollingInterface.h"
#include "UmbraEnemyCharacter.generated.h"

class AUmbraAIController;
class UBehaviorTree;

/**
 * 
 */
UCLASS()
class UMBRA_API AUmbraEnemyCharacter : public AUmbraBaseCharacter, public IInteractionInterface, public IPatrollingInterface
{
	GENERATED_BODY()

public:
	AUmbraEnemyCharacter();
	virtual void PossessedBy(AController* NewController) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void Interact(AActor* OtherActor) override;

	/** Patrolling Interface */
	virtual AActor* GetCurrentDestinationPoint() const override;
	virtual void IncrementCurrentDestinationPoint() override;
	virtual void ChoosePath() override;
	/** End Patrolling Interface */

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultTemperamentalAttributes;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Patrolling")
	TArray<FPathData> PatrollingPaths;
	
	UPROPERTY()
	TObjectPtr<AUmbraAIController> UmbraAIController;

private:
	int32 CurrentPathIndex = 0;
	int32 CurrentDestinationPointIndex = 0;
};
