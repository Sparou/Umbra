// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EQC_FindLostPlayer.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UEQC_FindLostPlayer : public UEnvQueryContext
{
	GENERATED_BODY()

protected:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
