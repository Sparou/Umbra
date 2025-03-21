// Copyrighted by Vorona Games


#include "AI/BTTask_LookAround.h"
#include "AIController.h"

UBTTask_LookAround::UBTTask_LookAround()
{
	bNotifyTick = true;
	NodeName = "LookAround";
}

EBTNodeResult::Type UBTTask_LookAround::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{	
	const AAIController* AIController = OwnerComp.GetAIOwner();
	if(!AIController) return EBTNodeResult::Failed;

	APawn* ControlledPawn = AIController->GetPawn();
	if(!ControlledPawn) return EBTNodeResult::Failed;
	
	OriginalRotation = ControlledPawn->GetActorRotation();
	TargetRotation = OriginalRotation;

	ElapsedTime = 0.f;
	SideTime = 0.f;
	bLookingRight = true;
	
	return EBTNodeResult::InProgress;
}

void UBTTask_LookAround::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ElapsedTime += DeltaSeconds;
	SideTime += DeltaSeconds;

	const AAIController* AIController = OwnerComp.GetAIOwner();
	if(!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if(!ControlledPawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (ElapsedTime >= LookAroundTime)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	if (SideTime >= OneSideAttentionTime)
	{
		SideTime = 0.f;
		bLookingRight = !bLookingRight;

		const float Angle = RotationAngle * (bLookingRight ? 1 : -1);
		TargetRotation = OriginalRotation + FRotator(0, Angle, 0);
	}

	ControlledPawn->SetActorRotation(
		FMath::RInterpTo(
			ControlledPawn->GetActorRotation(),
			TargetRotation,
			DeltaSeconds,
			RotationSpeed));
}