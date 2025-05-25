// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/CarryBodyAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Component/InteractionComponent.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/UmbraPlayerController.h"


bool UCarryBodyAbility::InitializeSourceCharacter()
{
	SourceCharacter = Cast<AUmbraPlayerCharacter>(GetAvatarActorFromActorInfo());
	
	if (!SourceCharacter)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[%s]: Invalid Initiator!"), *this->GetName());
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return false;
	}
	return true;
}

bool UCarryBodyAbility::InitializeTargetCharacter()
{
	UInteractionComponent* InteractionComponent = SourceCharacter->GetComponentByClass<UInteractionComponent>();

	if (!InteractionComponent)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[Assasination Ability]: Invalid Interaction Component!"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return false;
	}
	
	TargetCharacter = Cast<AUmbraBaseCharacter>(InteractionComponent->GetInteractionActor());

	if (!TargetCharacter)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[Assasination Ability]: Invalid Target!"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return false;
	}
	return true;
}

bool UCarryBodyAbility::ValidateActivationDistance()
{
	if (FVector::DistSquared(SourceCharacter->GetActorLocation(), TargetCharacter->GetActorLocation()) > ActivationDistance * ActivationDistance)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[%s]: Target is too far!"), *this->GetName());
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return false;
	}
	return true;
}

void UCarryBodyAbility::PibckupBody()
{
}

void UCarryBodyAbility::ReleaseBody()
{
}

void UCarryBodyAbility::OnMontageBlendingOut()
{
}
