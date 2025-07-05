// Copyrighted by Vorona Games


#include "Character/UmbraEnemyCharacter.h"
#include "UmbraCollisionChannels.h"
#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "AbilitySystem/UmbraEnemyAttributeSet.h"
#include "AI/UmbraAIController.h"
#include "AI/InteractingObject/UmbraAlarmBell.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

AUmbraEnemyCharacter::AUmbraEnemyCharacter()
{
	AttributeSet = CreateDefaultSubobject<UUmbraEnemyAttributeSet>("Attribute Set");
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Interaction, ECR_Block);
}

void AUmbraEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(!HasAuthority()) return;

	UmbraAIController = Cast<AUmbraAIController>(NewController);
	UmbraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	UmbraAIController->RunBehaviorTree(BehaviorTree);
	
}

const AUmbraAlarmBell* AUmbraEnemyCharacter::NearestAlarmBell() const
{
	AUmbraAlarmBell* NearestAlarmBell = nullptr;
	for(AUmbraAlarmBell* AlarmBell : AlarmBells)
	{
		if(!NearestAlarmBell ||
			(NearestAlarmBell &&
				this->GetDistanceTo(AlarmBell) < this->GetDistanceTo(NearestAlarmBell)))
		{
			NearestAlarmBell = AlarmBell;
		}		
	}
	return NearestAlarmBell;
}

void AUmbraEnemyCharacter::MulticastHandleDeath_Implementation()
{
	//UmbraAIController->BrainComponent->StopLogic("Death");
	if(UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(UmbraAIController->BrainComponent))
	{
		BehaviorTreeComponent->StopTree(EBTStopMode::Forced);
	}

	UmbraAIController->StopMovement();

	
	if(UmbraAIController->PerceptionComponent)
	{
		UmbraAIController->PerceptionComponent->OnTargetPerceptionUpdated.Clear();
		UmbraAIController->PerceptionComponent->SetSenseEnabled(UAISense_Sight::StaticClass(), false);
		UmbraAIController->PerceptionComponent->SetSenseEnabled(UAISense_Hearing::StaticClass(), false);
	}

	UmbraAIController->UnPossess();
	UmbraAIController->Destroy();

	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
	}
	
	Super::MulticastHandleDeath_Implementation();
}

AActor* AUmbraEnemyCharacter::GetCurrentDestinationPoint() const
{
	//Check index's variables validity
	if(PatrollingPaths.IsValidIndex(CurrentPathIndex) &&
		PatrollingPaths[CurrentPathIndex].PathPoints.IsValidIndex(CurrentDestinationPointIndex))
	{
		return PatrollingPaths[CurrentPathIndex].PathPoints[CurrentDestinationPointIndex];
	}
	return nullptr;
}

void AUmbraEnemyCharacter::IncrementCurrentDestinationPoint()
{
	//Check index's variables validity
	if(PatrollingPaths.IsValidIndex(CurrentPathIndex) &&
		PatrollingPaths[CurrentPathIndex].PathPoints.IsValidIndex(CurrentDestinationPointIndex + 1))
	{
		CurrentDestinationPointIndex++;
	}
	else
	{
		ChoosePath();
	}
}

void AUmbraEnemyCharacter::ChoosePath()
{
	CurrentPathIndex = FMath::RandRange(0, PatrollingPaths.Num() - 1);
	CurrentDestinationPointIndex = 0;
}

void AUmbraEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AlarmBells.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no bells"), *GetName());
	}
	
	for (AUmbraAlarmBell* AlarmBell : AlarmBells)
	{
		if(AlarmBell && !AlarmBell->OnBellRang.IsAlreadyBound(this, &AUmbraEnemyCharacter::OnAlarm))
		{
			AlarmBell->OnBellRang.AddDynamic(this, &AUmbraEnemyCharacter::OnAlarm);
			//UE_LOG(LogTemp, Warning, TEXT("%s"), *AlarmBell->GetName());
		}
	}
	
	ChoosePath();

	InitAbilityActorInfo();
}

void AUmbraEnemyCharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if(HasAuthority())
	{
		InitializeDefaultAttributes();
		AddCharacterAbilities();
	}
}

void AUmbraEnemyCharacter::InitializeDefaultAttributes() const
{
	Super::InitializeDefaultAttributes();
	ApplyEffectToSelf(DefaultTemperamentalAttributes, 1.f);
	UmbraAIController->InitializeBlackboardDefaultValues(Cast<UUmbraEnemyAttributeSet>(AttributeSet));
}

void AUmbraEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (AUmbraAlarmBell* AlarmBell : AlarmBells)
	{
		if(AlarmBell)
		{
			AlarmBell->OnBellRang.RemoveDynamic(this, &AUmbraEnemyCharacter::OnAlarm);
		}
	}
}

void AUmbraEnemyCharacter::OnAlarm(FVector BellLocation)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if(!AIController) return;
	
	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	if(!BlackboardComponent) return;

	BlackboardComponent->SetValueAsVector(TriggeredAlarmLocation, BellLocation);
}
