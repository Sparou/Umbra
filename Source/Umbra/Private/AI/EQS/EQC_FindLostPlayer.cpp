// Copyrighted by Vorona Games


#include "AI/EQS/EQC_FindLostPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"

void UEQC_FindLostPlayer::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	AActor* QueryOwner = Cast<AActor>(QueryInstance.Owner.Get());
	if(!QueryOwner) return;

	AAIController* AIController = Cast<AAIController>(QueryOwner);
	//in case of eqs testing pawn
	if(!AIController)
	{
		APawn* Pawn = Cast<APawn>(QueryOwner);
		if(Pawn)
		{
			AIController = Cast<AAIController>(Pawn->GetController());
		}
	}

	if(AIController)
	{
		UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
		if(BlackboardComponent)
		{
			FVector LastSeenLocation = BlackboardComponent->GetValueAsVector(FName("EnemyLocation"));
			ContextData.ValueType = UEnvQueryItemType_VectorBase::StaticClass();
			UEnvQueryItemType_Point::SetContextHelper(ContextData, LastSeenLocation);
			//ContextData.RawData.Add(new FVector(LastSeenLocation));
		}
	}
}
