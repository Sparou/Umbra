// Copyrighted by Vorona Games


#include "AI/BehaviorTree/Service/BTService_ChooseEnemy.h"
#include "AI/UmbraAIController.h"

void UBTService_ChooseEnemy::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	AUmbraAIController* AIController = Cast<AUmbraAIController>(OwnerComp.GetAIOwner());
	if(!AIController) return;

	AIController->ChooseEnemy();
}
