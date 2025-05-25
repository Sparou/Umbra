// Copyrighted by Vorona Games


#include "AI/BehaviorTree/Task/BTTask_FindNearestAlarmBell.h"

#include "AIController.h"
#include "AI/InteractingObject/UmbraAlarmBell.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/UmbraEnemyCharacter.h"

EBTNodeResult::Type UBTTask_FindNearestAlarmBell::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if(!AIController) return EBTNodeResult::Failed;

	AUmbraEnemyCharacter* Character = Cast<AUmbraEnemyCharacter>(AIController->GetPawn());
	if(!Character) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	if(!BlackboardComponent) return EBTNodeResult::Failed;

	BlackboardComponent->SetValueAsVector(BlackboardKey.SelectedKeyName, Character->NearestAlarmBell()->GetActorLocation());
	
	return EBTNodeResult::Succeeded;
}
