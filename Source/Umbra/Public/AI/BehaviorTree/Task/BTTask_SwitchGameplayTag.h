// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SwitchGameplayTag.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UBTTask_SwitchGameplayTag : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SwitchGameplayTag();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, Category = Node, meta = (ToolTip = "Apply tag if true, otherwise delete"))
	bool ToggleTag = true;

	UPROPERTY(EditAnywhere, Category = Node)
	FGameplayTag TagToToggle;
};
