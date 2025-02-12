// Copyrighted by Vorona Games


#include "AI/BTService_UpdateCurrentEnemyData.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UBTService_UpdateCurrentEnemyData::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                 float DeltaSeconds)
{
	//TODO: After losing player distanceToTarget should be changed on neutral value
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if(!BlackboardComponent) return;

	if(AActor* CurrentEnemy = Cast<AActor>(BlackboardComponent->GetValueAsObject(CurrentEnemySelector.SelectedKeyName)))
	{
		const float Distance = OwnerComp.GetAIOwner()->GetPawn()->GetDistanceTo(CurrentEnemy);
		BlackboardComponent->SetValueAsFloat(DistanceToTargetSelector.SelectedKeyName, Distance);
	}
}
