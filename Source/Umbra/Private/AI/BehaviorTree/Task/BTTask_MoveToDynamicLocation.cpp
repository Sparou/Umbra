// Copyrighted by Vorona Games


#include "AI/BehaviorTree/Task/BTTask_MoveToDynamicLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_MoveToDynamicLocation::UBTTask_MoveToDynamicLocation()
{
	NodeName = "Move To Dynamic Location";
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_MoveToDynamicLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	if(!BlackboardComponent || !AIController) return EBTNodeResult::Failed;

	//TODO: May be invalid?
	PreviousTargetLocation = BlackboardComponent->GetValueAsVector(TargetLocationKey.SelectedKeyName);
	bIsMoving = true;
	AIController->MoveToLocation(PreviousTargetLocation, AcceptanceRadius, true, true, true);
	
	return EBTNodeResult::InProgress;
}

void UBTTask_MoveToDynamicLocation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if(!bIsMoving) return;

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	if(!BlackboardComponent || !AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	APawn* AIPawn = AIController->GetPawn();
	if(!AIPawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	//TODO: May be invalid?
	const FVector CurrentTargetLocation = BlackboardComponent->GetValueAsVector(TargetLocationKey.SelectedKeyName);
	const float PreviousDistance = FVector::Dist(AIPawn->GetActorLocation(), PreviousTargetLocation);

	if(ShouldUpdatePath(CurrentTargetLocation) || PreviousDistance <= AcceptanceRadius)
	{
		PreviousTargetLocation = CurrentTargetLocation;
		const EPathFollowingRequestResult::Type Result = AIController->MoveToLocation(CurrentTargetLocation, AcceptanceRadius, true, true, true);

		//Check if there's no way
		if(Result == EPathFollowingRequestResult::Failed)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}
	}
	
	if (AIController->GetPathFollowingComponent()->DidMoveReachGoal())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
}

void UBTTask_MoveToDynamicLocation::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	bIsMoving = false;
}