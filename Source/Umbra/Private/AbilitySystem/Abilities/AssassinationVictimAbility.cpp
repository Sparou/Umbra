// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/AssassinationVictimAbility.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/UmbraEnemyCharacter.h"
#include "Components/CapsuleComponent.h"

void UAssassinationVictimAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                  const FGameplayEventData* TriggerEventData)
{
	AUmbraEnemyCharacter* SourceCharacter = Cast<AUmbraEnemyCharacter>(GetAvatarActorFromActorInfo());

	if (SourceCharacter->HasAuthority())
	{
		SourceCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SourceCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	if (SourceCharacter)
	{
		if (AAIController* AIController = Cast<AAIController>(SourceCharacter->GetController()))
		{
			AIController->StopMovement();
			AIController->BrainComponent->StopLogic("Stealth Victim");
			AIController->ClearFocus(EAIFocusPriority::Gameplay);
		}
	}
	
	const UAnimMontage* VictimAnimMontageConst = Cast<UAnimMontage>(TriggerEventData->OptionalObject);
	UAnimMontage* VictimAnimMontage = const_cast<UAnimMontage*>(VictimAnimMontageConst);
	
	if (ActorInfo->GetAnimInstance())
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		VictimAnimMontage,
		1.0f,
		NAME_None,
		false,
		1.0f);

		PlayMontageTask->OnCompleted.AddDynamic(this, &UAssassinationVictimAbility::OnMontageCompleted);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UAssassinationVictimAbility::OnMontageInterrupted);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UAssassinationVictimAbility::OnMontageCancelled);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &UAssassinationVictimAbility::OnMontageBlendingOut);
		PlayMontageTask->ReadyForActivation();
	}
}


void UAssassinationVictimAbility::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	UE_LOG(LogTemp, Warning, TEXT("Stealth kill completed"));
}

void UAssassinationVictimAbility::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	UE_LOG(LogTemp, Warning, TEXT("Stealth kill completed"));
}

void UAssassinationVictimAbility::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	UE_LOG(LogTemp, Warning, TEXT("Stealth kill completed"));
}

void UAssassinationVictimAbility::OnMontageBlendingOut()
{
	if(GetAvatarActorFromActorInfo()->HasAuthority())
	{
		AUmbraBaseCharacter* AvatarCharacter = Cast<AUmbraBaseCharacter>(GetAvatarActorFromActorInfo());
		AvatarCharacter->MulticastHandleDeath();
	}
}
