// Copyrighted by Vorona Games


#include "AI/UmbraAIController.h"

#include "AI/UmbraAIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
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
}

void AUmbraAIController::OnPercepted(AActor* SourceActor, const FAIStimulus Stimulus)
{
	FString Message = "Stimuled by" + Stimulus.Type.Name.ToString() + "strength = " + FString::SanitizeFloat(Stimulus.Strength);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
	
	if(Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
	{
		if(Stimulus.WasSuccessfullySensed())
		{
			Blackboard->SetValueAsObject("CurrentEnemy", SourceActor);
			Blackboard->SetValueAsVector("PerceptionSourceLocation", Stimulus.StimulusLocation);
		}
		else
		{
			Blackboard->SetValueAsObject("CurrentEnemy", nullptr);
			Blackboard->SetValueAsVector("PerceptionSourceLocation", Stimulus.StimulusLocation);
		}
	}
}
