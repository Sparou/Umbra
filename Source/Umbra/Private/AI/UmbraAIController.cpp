// Copyrighted by Vorona Games


#include "AI/UmbraAIController.h"
#include "AbilitySystem/UmbraEnemyAttributeSet.h"
#include "AI/UmbraAIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

AUmbraAIController::AUmbraAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
	check(Blackboard);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	check(BehaviorTreeComponent);

	UmbraAIPerceptionComponent = CreateDefaultSubobject<UUmbraAIPerceptionComponent>("UmbraAIPerceptionComponent");
	check(UmbraAIPerceptionComponent);
	SetPerceptionComponent(*UmbraAIPerceptionComponent);
	
	UmbraAIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AUmbraAIController::OnPercepted);
	
	//NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>("NoiseEmitter");
	//NoiseEmitter->bAutoActivate = true;
}

bool AUmbraAIController::InitializeBlackboardDefaultValues(const UUmbraEnemyAttributeSet* AttributeSet) const
{
	if(!AttributeSet) return false;
	//TODO: check set variables on validity
	
	Blackboard->SetValueAsFloat(Belligerence, AttributeSet->GetAggressiveness());
	Blackboard->SetValueAsFloat(Fear, AttributeSet->GetFearfulness());
	Blackboard->SetValueAsFloat(EstimatedWinChance, AttributeSet->GetVisualAnalysis());

	return true;
}

void AUmbraAIController::OnPercepted(AActor* SourceActor, const FAIStimulus Stimulus)
{
	FString Message = "Stimuled by" + Stimulus.Type.Name.ToString() + "strength = " + FString::SanitizeFloat(Stimulus.Strength);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);

	if(Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
	{
		if(Stimulus.WasSuccessfullySensed())
		{
			Blackboard->SetValueAsObject(CurrentEnemy, SourceActor);
			Blackboard->SetValueAsVector(EnemyLocation, Stimulus.StimulusLocation);
		}
		else
		{
			Blackboard->SetValueAsObject(LostEnemy, Blackboard->GetValueAsObject(CurrentEnemy));
			Blackboard->SetValueAsObject(CurrentEnemy, nullptr);
			Blackboard->SetValueAsVector(EnemyLocation, Stimulus.StimulusLocation);
		}
	}

	if(Stimulus.Type == UAISense::GetSenseID(UAISense_Hearing::StaticClass()))
	{
		if(Stimulus.WasSuccessfullySensed())
		{
			Blackboard->SetValueAsVector(SoundLocation, Stimulus.StimulusLocation);	
		}
		else
		{
		}
	}
	
}
