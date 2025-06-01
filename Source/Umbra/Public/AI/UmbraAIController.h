// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UmbraAIController.generated.h"

class UDA_EnemyChoicePriority;
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
	bool IsVisible;

	UPROPERTY()
	float LastSeenTime;

	UPROPERTY()
	FVector LastKnownLocation;

	//TODO: increment threatLevel in Damage handler
	/// <summary>
	///	Threat level is calculated as the number of successful punches from the enemy
	/// </summary>
	UPROPERTY()
	float ThreatLevel;

	FEnemyData() : IsVisible(true), LastSeenTime(0.f), ThreatLevel(0)
	{
	}

	FEnemyData(const FVector& ActorLocation, const float LastSeen, const bool Visible = true, const float ThreatLevel = 0)
		: IsVisible(Visible),
		  LastSeenTime(LastSeen),
		  LastKnownLocation(ActorLocation),
		  ThreatLevel(ThreatLevel)
	{
	}
};


UCLASS()
class UMBRA_API AUmbraAIController : public AAIController
{
	GENERATED_BODY()

public:
	AUmbraAIController();

	UPROPERTY()
	TMap<AActor*, FEnemyData> KnownEnemies;
	
	bool InitializeBlackboardDefaultValues(const UUmbraEnemyAttributeSet* AttributeSet) const;

	bool ReactToEvent(const FName EventName);
	bool ChooseEnemy();

	virtual void BeginPlay() override;

protected:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY()
	TObjectPtr<UUmbraAIPerceptionComponent> UmbraAIPerceptionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Emotion")
	TObjectPtr<UDataTable> EmotionReactionTable;

	UPROPERTY(EditDefaultsOnly, Category = "Emotion")
	TObjectPtr<UCurveTable> EmotionModifierTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI target selection")
	UDA_EnemyChoicePriority* PriorityWeightsData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (ToolTip = "the time it takes for the AI to forget the enemy's location"))
	float ForgettingTime = 5.f; 

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

	UPROPERTY(EditDefaultsOnly, Category = "Event")
	FName SeeCorpse = "SeeCorpse";
#pragma endregion
	
private:
	UFUNCTION()
	void OnPercepted(AActor* SourceActor, const FAIStimulus Stimulus);

	UFUNCTION()
	void OnGameplayTagChanged(const FGameplayTag& Tag, bool bAdded);
};
