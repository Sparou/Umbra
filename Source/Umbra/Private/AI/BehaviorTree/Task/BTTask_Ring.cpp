// Copyrighted by Vorona Games


#include "AI/BehaviorTree/Task/BTTask_Ring.h"

#include "AIController.h"
#include "AI/InteractingObject/UmbraAlarmBell.h"
#include "Character/UmbraEnemyCharacter.h"

EBTNodeResult::Type UBTTask_Ring::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if(!AIController) return EBTNodeResult::Failed;

	AUmbraEnemyCharacter* Character = Cast<AUmbraEnemyCharacter>(AIController->GetPawn());
	if(!Character) return EBTNodeResult::Failed;

	const AUmbraAlarmBell* NearestAlarmBell = Character->NearestAlarmBell();
	if(!NearestAlarmBell) return EBTNodeResult::Failed;

	float Distance = FVector::Dist(Character->GetActorLocation(), NearestAlarmBell->GetActorLocation());
	//DrawDebugLine(GetWorld(), Character->GetActorLocation(), NearestAlarmBell->GetActorLocation(), FColor::Red, true, -1, 0, 5);
	if(Distance > AcceptanceRadius)
	{
		return EBTNodeResult::Failed;
	}

	NearestAlarmBell->RingBell();
	
	return EBTNodeResult::Succeeded;
}
