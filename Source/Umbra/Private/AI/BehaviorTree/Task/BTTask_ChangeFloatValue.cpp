// Copyrighted by Vorona Games


#include "AI/BehaviorTree/Task/BTTask_ChangeFloatValue.h"

#include <string>

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ChangeFloatValue::UBTTask_ChangeFloatValue()
{
	NodeName = "Change Float Value";
}

EBTNodeResult::Type UBTTask_ChangeFloatValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if(!BlackboardComponent) return EBTNodeResult::Failed;

	const float CurrentValue = BlackboardComponent->GetValueAsFloat(BlackboardKey.SelectedKeyName);
	BlackboardComponent->SetValueAsFloat(BlackboardKey.SelectedKeyName, CurrentValue + AddNumber);

	return EBTNodeResult::Succeeded;
}

FString UBTTask_ChangeFloatValue::GetStaticDescription() const
{
	const FString KeyName = BlackboardKey.SelectedKeyName.ToString();
	const FString ValueString = FString::SanitizeFloat(AddNumber);

	return FString::Printf(TEXT("Add %s to %s"), *ValueString, *KeyName);
}
