// Copyrighted by Vorona Games


#include "AI/BehaviorTree/Task/BTTask_SwitchGameplayTag.h"

#include "AIController.h"
#include "Character/Component/TagManager.h"

UBTTask_SwitchGameplayTag::UBTTask_SwitchGameplayTag()
{
	NodeName = "Switch Gameplay Tag";
}

FString UBTTask_SwitchGameplayTag::GetStaticDescription() const
{
	const FString Operation = ToggleTag ? "Apply" : "Remove";
	const FString Tag = TagToToggle.GetTagName().ToString();
	return FString::Printf(TEXT("%s %s tag"), *Operation, *Tag);
}

EBTNodeResult::Type UBTTask_SwitchGameplayTag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if(!AIController) return EBTNodeResult::Failed;

	APawn* AIPawn = AIController->GetPawn();
	if(!AIPawn) return EBTNodeResult::Failed;

	UTagManager* TagManager = AIPawn->FindComponentByClass<UTagManager>();
	if(!TagManager) return EBTNodeResult::Failed;
	
	if(ToggleTag) TagManager->AddTag(TagToToggle);
	else TagManager->RemoveTag(TagToToggle);
	
	return EBTNodeResult::Succeeded;
}
