// Copyrighted by Vorona Games


#include "AI/UmbraAIController.h"
#include "AbilitySystem/UmbraEnemyAttributeSet.h"
#include "AI/UmbraAIPerceptionComponent.h"
#include "AI/Data/FEmotionReactionRow.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/UmbraEnemyCharacter.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

AUmbraAIController::AUmbraAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
	check(Blackboard);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	check(BehaviorTreeComponent);

	UmbraAIPerceptionComponent = CreateDefaultSubobject<UUmbraAIPerceptionComponent>("UmbraAIPerceptionComponent");
	check(UmbraAIPerceptionComponent);
	SetPerceptionComponent(*UmbraAIPerceptionComponent);
	
	UmbraAIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AUmbraAIController::OnPercepted);
	
	//NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>("NoiseEmitter");
	//NoiseEmitter->bAutoActivate = true;
}

bool AUmbraAIController::InitializeBlackboardDefaultValues(const UUmbraEnemyAttributeSet* AttributeSet) const
{
	if(!AttributeSet) return false;
	//TODO: check set variables on validity
	
	Blackboard->SetValueAsFloat(Belligerence, AttributeSet->GetAggressiveness());
	Blackboard->SetValueAsFloat(Fear, AttributeSet->GetFearfulness());
	Blackboard->SetValueAsFloat(EstimatedWinChance, AttributeSet->GetVisualAnalysis());

	return true;
}

bool AUmbraAIController::ReactToEvent(FName EventName)
{
	if(!EmotionReactionTable) return false;
	if(!EmotionModifierTable) return false;

	const FEmotionReactionRow* ReactionRow = EmotionReactionTable->FindRow<FEmotionReactionRow>(EventName, TEXT("EmotionReaction"));
	if(!ReactionRow) return false;
	
	AUmbraEnemyCharacter* EnemyCharacter = Cast<AUmbraEnemyCharacter>(GetPawn());
	if(!EnemyCharacter) return false;

	const UAbilitySystemComponent* AbilitySystemComponent = EnemyCharacter->GetAbilitySystemComponent();
	if(!AbilitySystemComponent) return false;

	const UUmbraEnemyAttributeSet* AttributeSet = AbilitySystemComponent->GetSet<UUmbraEnemyAttributeSet>();
	if(!AttributeSet) return false;

	const FRealCurve* AggressivenessModifierCurve = EmotionModifierTable->FindCurve(FName("Aggressiveness"), TEXT("EmotionModifier"));
	if(!AggressivenessModifierCurve) return false;

	float AggressivenessModifier = AggressivenessModifierCurve->Eval(AttributeSet->GetAggressiveness());

	FRealCurve* FearfulnessModifierCurve = EmotionModifierTable->FindCurve(FName("Fearfulness"), TEXT("EmotionModifier"));
	if(!FearfulnessModifierCurve) return false;

	float FearfulnessModifier = FearfulnessModifierCurve->Eval(AttributeSet->GetFearfulness());

	const float NewBelligerenceValue = Blackboard->GetValueAsFloat(Belligerence) + ReactionRow->BelligerenceDelta * AggressivenessModifier;
	const float NewFearValue = Blackboard->GetValueAsFloat(Fear) + ReactionRow->FearDelta * FearfulnessModifier;
	Blackboard->SetValueAsFloat(Belligerence, NewBelligerenceValue);
	Blackboard->SetValueAsFloat(Fear, NewFearValue);	
	
	return true;
}

void AUmbraAIController::OnPercepted(AActor* SourceActor, const FAIStimulus Stimulus)
{
	FString Message = "Stimuled by" + Stimulus.Type.Name.ToString() + "strength = " + FString::SanitizeFloat(Stimulus.Strength);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);

	if(Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
	{
		if(Stimulus.WasSuccessfullySensed())
		{
			if(AUmbraPlayerCharacter* PlayerActor = Cast<AUmbraPlayerCharacter>(SourceActor))
			{
				//TODO: check if bot sees player after ending invisibility when stimulus was already received 
				if(!PlayerActor->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(InvisibleTag)))
				{
					Blackboard->SetValueAsBool(EverSeenEnemy, true);
					Blackboard->SetValueAsObject(CurrentEnemy, SourceActor);
					Blackboard->SetValueAsVector(EnemyLocation, Stimulus.StimulusLocation);
				}
			}
		}
		else
		{
			Blackboard->SetValueAsObject(LostEnemy, Blackboard->GetValueAsObject(CurrentEnemy));
			Blackboard->SetValueAsObject(CurrentEnemy, nullptr);
			Blackboard->SetValueAsVector(EnemyLocation, Stimulus.StimulusLocation);
		}
	}

	if(Stimulus.Type == UAISense::GetSenseID(UAISense_Hearing::StaticClass()))
	{
		if(Stimulus.WasSuccessfullySensed())
		{
			Blackboard->SetValueAsVector(SoundLocation, Stimulus.StimulusLocation);

			float NewFearValue = Blackboard->GetValueAsFloat(Fear);
			ReactToEvent("HearNoise");
			NewFearValue = NewFearValue;
			//Blackboard->SetValueAsFloat(Fear, );
		}
		else
		{
		}
	}
	
}
