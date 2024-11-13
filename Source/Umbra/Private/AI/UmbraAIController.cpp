// Copyrighted by Vorona Games


#include "AI/UmbraAIController.h"

#include "AI/UmbraAIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AUmbraAIController::AUmbraAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
	check(Blackboard);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	check(BehaviorTreeComponent);

	UmbraAIPerceptionComponent = CreateDefaultSubobject<UUmbraAIPerceptionComponent>("UmbraAIPerceptionComponent");
	check(UmbraAIPerceptionComponent);
	SetPerceptionComponent(*UmbraAIPerceptionComponent);
}
