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

USTRUCT(BlueprintType)
struct UMBRA_API FEnemyData
{

	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY()
	bool IsVisible;

	UPROPERTY()
	float LastSeenTime;

	UPROPERTY()
	FVector LastKnownLocation;

	FEnemyData() : IsVisible(true), LastSeenTime(0.f) {}

	FEnemyData(AActor* Actor, const FVector& ActorLocation, const bool Visible = true, const float LastSeen = 0.f)
		: TargetActor(Actor),
		  IsVisible(Visible),
		  LastSeenTime(LastSeen),
		  LastKnownLocation(ActorLocation)
	{}
};

FORCEINLINE bool operator==(const FEnemyData& FirstEnemyData, const FEnemyData& SecondEnemyData)
{
	return FirstEnemyData.TargetActor == SecondEnemyData.TargetActor;
}

FORCEINLINE uint32 GetTypeHash(const FEnemyData& EnemyData)
{
	return GetTypeHash(EnemyData.TargetActor);
}

UCLASS()
class UMBRA_API AUmbraAIController : public AAIController
{
	GENERATED_BODY()

public:
	AUmbraAIController();
	bool InitializeBlackboardDefaultValues(const UUmbraEnemyAttributeSet* AttributeSet) const;

	bool ReactToEvent(const FName EventName);

protected:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY()
	TObjectPtr<UUmbraAIPerceptionComponent> UmbraAIPerceptionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Emotion")
	TObjectPtr<UDataTable> EmotionReactionTable;

	UPROPERTY(EditDefaultsOnly, Category = "Emotion")
	TObjectPtr<UCurveTable> EmotionModifierTable;

#pragma region Blackboard value names
	UPROPERTY(EditDefaultsOnly, Category = "Blackboard|Patrolling")
	FName NextPatrollingPoint = "NextPatrollingPoint";

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard|Vision")
	FName EverSeenEnemy = "EverSeenEnemy";

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

#pragma region GameplayTag names
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTag")
	FName InvisibleTag = "Ability.Invisibility";
#pragma endregion

#pragma region Event names
	UPROPERTY(EditDefaultsOnly, Category = "Event")
	FName SeeAlly = "SeeAlly";

	UPROPERTY(EditDefaultsOnly, Category = "Event")
	FName SeeEnemy = "SeeEnemy";

	UPROPERTY(EditDefaultsOnly, Category = "Event")
	FName HearNoise = "HearNoise";
#pragma endregion
	
private:
	UPROPERTY()
	TSet<FEnemyData> KnownEnemies;
	
	UFUNCTION()
	void OnPercepted(AActor* SourceActor, const FAIStimulus Stimulus);
};
