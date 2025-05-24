// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AI/BehaviorTree/Task/BTTask_ClearBlackboardVariable.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EQC_MaintainDistanceFromActor.generated.h"

UCLASS()
class UMBRA_API UEQC_MaintainDistanceFromActor : public UEnvQueryContext
{
	GENERATED_BODY()

public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CurrentEnemyKey;
};
