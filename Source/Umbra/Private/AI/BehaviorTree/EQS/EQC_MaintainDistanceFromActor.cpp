// Copyrighted by Vorona Games


#include "AI/BehaviorTree/EQS/EQC_MaintainDistanceFromActor.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEQC_MaintainDistanceFromActor::ProvideContext(FEnvQueryInstance& QueryInstance,
                                                    FEnvQueryContextData& ContextData) const
{
	const UObject* QueryOwner = QueryInstance.Owner.Get();
	const AAIController* AIController = Cast<AAIController>(QueryOwner);
	if (!AIController) return;

	const UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	if (!BlackboardComponent) return;

	const AActor* CurrentEnemy = Cast<AActor>(BlackboardComponent->GetValueAsObject(CurrentEnemyKey.SelectedKeyName));
	if (CurrentEnemy) return;
	
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, CurrentEnemy);
}
