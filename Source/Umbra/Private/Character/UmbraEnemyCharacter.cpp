// Copyrighted by Vorona Games


#include "Character/UmbraEnemyCharacter.h"
#include "AbilitySystem/UmbraEnemyAttributeSet.h"
#include "AI/UmbraAIController.h"
#include "AI/InteractingObject/UmbraAlarmBell.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

AUmbraEnemyCharacter::AUmbraEnemyCharacter()
{
	AttributeSet = CreateDefaultSubobject<UUmbraEnemyAttributeSet>("Attribute Set");
}

void AUmbraEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(!HasAuthority()) return;

	UmbraAIController = Cast<AUmbraAIController>(NewController);
	UmbraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	UmbraAIController->RunBehaviorTree(BehaviorTree);
}

void AUmbraEnemyCharacter::Interact(AActor* OtherActor)
{
	bInInteraction = true;
	GEngine->AddOnScreenDebugMessage(0, 3.f, FColor::Blue, FString("Interacted with object"));
	bInInteraction = false;
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

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUmbraAlarmBell::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (AUmbraAlarmBell* Bell = Cast<AUmbraAlarmBell>(Actor))
		{
			AlarmBells.Add(Bell);
		}
	}
	
	for (AUmbraAlarmBell* AlarmBell : AlarmBells)
	{
		if(AlarmBell && !AlarmBell->OnBellRang.IsAlreadyBound(this, &AUmbraEnemyCharacter::OnAlarm))
		{
			AlarmBell->OnBellRang.AddDynamic(this, &AUmbraEnemyCharacter::OnAlarm);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *AlarmBell->GetName());
		}
	}
	
	ChoosePath();
}

void AUmbraEnemyCharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	//Cast<UmbraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	if(HasAuthority())
	{
		InitializeDefaultAttributes();
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
