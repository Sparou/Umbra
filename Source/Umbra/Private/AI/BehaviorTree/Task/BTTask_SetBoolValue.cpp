// Copyrighted by Vorona Games


#include "AI/BehaviorTree/Task/BTTask_SetBoolValue.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetBoolValue::UBTTask_SetBoolValue()
{
	NodeName = "Set Bool Value";
}

EBTNodeResult::Type UBTTask_SetBoolValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if(!BlackboardComponent) return EBTNodeResult::Failed;

	BlackboardComponent->SetValueAsBool(BlackboardKey.SelectedKeyName, Value);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_SetBoolValue::GetStaticDescription() const
{
	const FString KeyName = BlackboardKey.SelectedKeyName.ToString();
	const FString KeyValue = Value ? "true" : "false";
	return FString::Printf(TEXT("Set %s to %s"), *KeyName, *KeyValue);
}
