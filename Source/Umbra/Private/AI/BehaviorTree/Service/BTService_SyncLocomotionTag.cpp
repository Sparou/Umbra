// Copyrighted by Vorona Games


#include "AI/BehaviorTree/Service/BTService_SyncLocomotionTag.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Component/TagManager.h"

UBTService_SyncLocomotionTag::UBTService_SyncLocomotionTag()
{
	bNotifyTick = true;
	NodeName = "Sync Locomotion Tag";
}

void UBTService_SyncLocomotionTag::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTService_SyncMemory* Memory = reinterpret_cast<FBTService_SyncMemory*>(NodeMemory);
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* AIPawn = AIController ? AIController->GetPawn() : nullptr;

	if (!Memory || !BlackboardComponent || !AIController || !AIPawn) return;

	const bool bIsRunning = BlackboardComponent->GetValueAsBool(IsRunningKey.SelectedKeyName);
	//UE_LOG(LogTemp, Warning, TEXT("Memory = %hs"), Memory->bIsRunning ? "true" : "false");
	if(bIsRunning != Memory->bIsRunning)
	{
		UTagManager* TagManager = AIPawn->FindComponentByClass<UTagManager>();
		if(!TagManager)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tag manager isn't found"));
			return;
		}
		UE_LOG(LogTemp, Warning, TEXT("Log after TagManager check"));
		if(bIsRunning)
		{
			UE_LOG(LogTemp, Warning, TEXT("Log bIsRunning"));
			TagManager->AddTag(RunningTag);
			TagManager->RemoveTag(WalkingTag);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Log !bIsRunning"));
			TagManager->AddTag(WalkingTag);
			TagManager->RemoveTag(RunningTag);
		}
		UE_LOG(LogTemp, Warning, TEXT("Log after bIsRunning check"));
 		Memory->bIsRunning = bIsRunning;
	}
}

uint16 UBTService_SyncLocomotionTag::GetInstanceMemorySize() const
{
	return sizeof(FBTService_SyncMemory);
}

void UBTService_SyncLocomotionTag::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTMemoryInit::Type InitType) const
{
	FBTService_SyncMemory* Memory = reinterpret_cast<FBTService_SyncMemory*>(NodeMemory);
	Memory->bIsRunning = true;
}
