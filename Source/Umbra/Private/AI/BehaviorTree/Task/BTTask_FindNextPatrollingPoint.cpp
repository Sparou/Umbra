// Copyrighted by Vorona Games


#include "AI/BehaviorTree/Task/BTTask_FindNextPatrollingPoint.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Interface/PatrollingInterface.h"

UBTTask_FindNextPatrollingPoint::UBTTask_FindNextPatrollingPoint()
{
	NodeName = "Find Next Patrolling Point"; 
}

EBTNodeResult::Type UBTTask_FindNextPatrollingPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	if(!AIController) return EBTNodeResult::Failed;

	APawn* ControlledPawn = AIController->GetPawn();
	if(!ControlledPawn) return EBTNodeResult::Failed;

	IPatrollingInterface* PatrollingInterface = Cast<IPatrollingInterface>(ControlledPawn);
	if(!PatrollingInterface) return EBTNodeResult::Failed;
	
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if(!BlackboardComponent) return EBTNodeResult::Failed;

	BlackboardComponent->SetValueAsVector(CurrentDestinationPoint.SelectedKeyName, PatrollingInterface->GetCurrentDestinationPoint()->GetActorLocation());
	PatrollingInterface->IncrementCurrentDestinationPoint();
	
	return EBTNodeResult::Succeeded;
}
