// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Character/UmbraBaseCharacter.h"
#include "Interaction/InteractionInterface.h"
#include "Interface/PatrollingInterface.h"
#include "UmbraEnemyCharacter.generated.h"

class AUmbraAlarmBell;
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

	const AUmbraAlarmBell* NearestAlarmBell() const;

	/** Patrolling Interface */
	virtual AActor* GetCurrentDestinationPoint() const override;
	virtual void IncrementCurrentDestinationPoint() override;
	virtual void ChoosePath() override;
	/** End Patrolling Interface */

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultTemperamentalAttributes;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	//TODO: think whether u need to move patrollin n alarm logic to controller  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Patrolling")
	TArray<FPathData> PatrollingPaths;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alarm")
	TArray<AUmbraAlarmBell*> AlarmBells;
	
	UPROPERTY()
	TObjectPtr<AUmbraAIController> UmbraAIController;

	/*Blackboard value names*/
	UPROPERTY(EditDefaultsOnly, Category = "Blackboard|Alarm")
	FName TriggeredAlarmLocation = "TriggeredAlarmLocation";

private:
	int32 CurrentPathIndex = 0;
	int32 CurrentDestinationPointIndex = 0;

	UFUNCTION()
	void OnAlarm(FVector BellLocation);
};
