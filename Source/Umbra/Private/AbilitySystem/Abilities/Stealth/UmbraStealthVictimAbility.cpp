// Copyrighted by Vorona Games

#include "AbilitySystem/Abilities/Stealth/UmbraStealthVictimAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/UmbraPlayerCharacter.h"

void UUmbraStealthVictimAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{

	UE_LOG(LogTemp, Log, TEXT("Victim ability Started!"));
	
	AUmbraPlayerCharacter* AvatarCharacter = Cast<AUmbraPlayerCharacter>(GetAvatarActorFromActorInfo());

	//TODO: Добавить отключение контроллера персонажам, подконтрольным ИИ.
	
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

		PlayMontageTask->OnCompleted.AddDynamic(this, &UUmbraStealthVictimAbility::OnMontageCompleted);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UUmbraStealthVictimAbility::OnMontageInterrupted);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UUmbraStealthVictimAbility::OnMontageCancelled);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &UUmbraStealthVictimAbility::OnMontageBlendingOut);
		PlayMontageTask->ReadyForActivation();
	}

}

void UUmbraStealthVictimAbility::OnMontageCompleted()
{
	//EnableMovement();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	UE_LOG(LogTemp, Warning, TEXT("Stealth kill completed"));
}

void UUmbraStealthVictimAbility::OnMontageInterrupted()
{
	//EnableMovement();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	UE_LOG(LogTemp, Warning, TEXT("Stealth kill interrupted"));
}

void UUmbraStealthVictimAbility::OnMontageCancelled()
{
	//EnableMovement();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	UE_LOG(LogTemp, Warning, TEXT("Stealth kill cancelled"));
}

void UUmbraStealthVictimAbility::OnMontageBlendingOut()
{
	if(GetAvatarActorFromActorInfo()->HasAuthority())
	{
		AUmbraPlayerCharacter* AvatarCharacter = Cast<AUmbraPlayerCharacter>(GetAvatarActorFromActorInfo());
		AvatarCharacter->MulticastHandleDeath();
	}
}

void UUmbraStealthVictimAbility::EnableMovement() const
{
	AUmbraPlayerCharacter* AvatarCharacter = Cast<AUmbraPlayerCharacter>(GetAvatarActorFromActorInfo());
	APlayerController* AvatarController = Cast<APlayerController>(AvatarCharacter->GetController());
	//InstigatorController->EnableInput(InstigatorController);
}


