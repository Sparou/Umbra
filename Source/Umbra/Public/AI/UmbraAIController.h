// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UmbraAIController.generated.h"

class UUmbraAIPerceptionComponent;
class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class UMBRA_API AUmbraAIController : public AAIController
{
	GENERATED_BODY()

public:
	AUmbraAIController();

protected:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY()
	TObjectPtr<UUmbraAIPerceptionComponent> UmbraAIPerceptionComponent;
};
