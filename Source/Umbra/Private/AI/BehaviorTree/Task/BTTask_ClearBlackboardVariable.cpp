// Copyrighted by Vorona Games


#include "AI/BehaviorTree/Task/BTTask_ClearBlackboardVariable.h"

#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_ClearBlackboardVariable::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if(!BlackboardComponent) return EBTNodeResult::Failed;

	//BlackboardComponent->SetValueAsVector(BlackboardKey.SelectedKeyName, FVector::ZeroVector);
	BlackboardComponent->ClearValue(BlackboardKey.SelectedKeyName);
	
	return EBTNodeResult::Succeeded;
}
