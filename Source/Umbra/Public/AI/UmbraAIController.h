// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UmbraAIController.generated.h"

class UUmbraEnemyAttributeSet;
struct FAIStimulus;
class UUmbraAIPerceptionComponent;
class UBehaviorTreeComponent;
class UPawnNoiseEmitterComponent;
/**
 * 
 */
UCLASS()
class UMBRA_API AUmbraAIController : public AAIController
{
	GENERATED_BODY()

public:
	AUmbraAIController();
	bool InitializeBlackboardDefaultValues(const UUmbraEnemyAttributeSet* AttributeSet) const;

protected:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY()
	TObjectPtr<UUmbraAIPerceptionComponent> UmbraAIPerceptionComponent;
	
	// UPROPERTY(VisibleAnywhere, Category = "AI")
	// TObjectPtr<UPawnNoiseEmitterComponent> NoiseEmitter;

#pragma region Blackboard value names
	UPROPERTY(EditDefaultsOnly, Category = "Blackboard|Patrolling")
	FName NextPatrollingPoint = "NextPatrollingPoint";

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard|Vision")
	FName EnemyLocation = "EnemyLocation";

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard|Vision")
	FName CurrentEnemy = "CurrentEnemy";

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard|Vision")
	FName DistanceToTarget = "DistanceToTarget";

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard|Vision")
	FName LostEnemy = "LostEnemy";

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard|MentalAttributes")
	FName Fear = "Fear";

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard|MentalAttributes")
	FName Belligerence = "Belligerence";

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard|MentalAttributes")
	FName EstimatedWinChance = "EstimatedWinChance";

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard|Hear")
	FName SoundLocation = "SoundLocation";
#pragma endregion
	
private:
	UFUNCTION()
	void OnPercepted(AActor* SourceActor, const FAIStimulus Stimulus);
};
