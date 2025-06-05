// Copyrighted by Vorona Games

#include "AbilitySystem/Abilities/AssassinationAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MotionWarpingComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/Abilities/GameplayAbilitiesFunctionLibrary.h"
#include "Character/Component/InteractionComponent.h"
#include "Character/Component/TraversalComponent.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Player/UmbraPlayerController.h"

DEFINE_LOG_CATEGORY(UmbraAbilitiesLog)

void UAssassinationAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	if (!InitializeSourceCharacter()) return;
	if (!InitializeTargetCharacter()) return;
	if (!ValidateActivationDistance()) return;

	if (!InitializeAssassinationData()) return;

	if (AUmbraPlayerController* UmbraPlayerController = Cast<AUmbraPlayerController>(SourceCharacter->GetController()))
	{
		UmbraPlayerController->SwitchToCameraOnlyContext();
	}
	
	SendEventToTarget();
	StartAssassination();
}

void UAssassinationAbility::StartAssassination()
{
	SourceCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	if (UMotionWarpingComponent* MotionWarping = SourceCharacter->FindComponentByClass<UMotionWarpingComponent>())
	{
		FVector WarpLocation = TargetCharacter->GetActorTransform().TransformPosition(AssassinationData.KillerStarterLocation);

		FVector Direction = TargetCharacter->GetActorLocation() - WarpLocation;
		Direction.Z = 0;
		Direction.Normalize();
		
		FRotator WarpRotation = Direction.Rotation();
		MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("AssassinationWarp", WarpLocation, WarpRotation);
	}
	
	UAbilityTask_PlayMontageAndWait* InitiatorTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		AssassinationData.KillerMontage,
		1.f,
		NAME_None,
		false,
		1.f);

	InitiatorTask->OnBlendOut.AddDynamic(this, &UAssassinationAbility::OnInitiatorMontageBlendOut);
	InitiatorTask->OnCompleted.AddDynamic(this, &UAssassinationAbility::OnInitiatorMontageCompleted);

	InitiatorTask->ReadyForActivation();
}

void UAssassinationAbility::SendEventToTarget()
{
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter))
	{
		FGameplayTag AbilityTag = FUmbraGameplayTags::Get().Ability_Stealth_Victim;

		FGameplayEventData EventData;
		EventData.EventTag = AbilityTag;
		EventData.Instigator = GetAvatarActorFromActorInfo();
		EventData.Target = TargetCharacter;
		EventData.OptionalObject = AssassinationData.VictimMontage;
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetCharacter, AbilityTag, EventData);
		UE_LOG(LogTemp, Log, TEXT("Event was sent!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Stealth Kill Ability: Target actor doesn't have ability system component!"));
		EnableMovement();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UAssassinationAbility::OnInitiatorMontageCompleted()
{
	EnableMovement();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAssassinationAbility::OnInitiatorMontageBlendOut()
{
	SourceCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

bool UAssassinationAbility::InitializeSourceCharacter()
{
	SourceCharacter = Cast<AUmbraPlayerCharacter>(GetAvatarActorFromActorInfo());
	
	if (!SourceCharacter)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[Assasination Ability]: Invalid Initiator!"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return false;
	}
	return true;
}

bool UAssassinationAbility::InitializeTargetCharacter()
{
	UInteractionComponent* InteractionComponent = SourceCharacter->GetComponentByClass<UInteractionComponent>();

	if (!InteractionComponent)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[Assasination Ability]: Invalid Interaction Component!"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return false;
	}
	
	TargetCharacter = InteractionComponent->GetInteractionActor();

	if (!TargetCharacter)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[Assasination Ability]: Invalid Target!"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return false;
	}
	return true;
}

bool UAssassinationAbility::InitializeAssassinationData()
{
	AssassinationData = SourceCharacter->GetAssassinationsData()->GetRandomAssassinationDataFromAbility(
		GetCurrentActivationInfo(),
		UGameplayAbilitiesFunctionLibrary::GetActorPositionRelativeToTarget(SourceCharacter, TargetCharacter, HeightDifferenceThreshold),
		RandomSeedMultiplier);
	
	if (!AssassinationData.KillerMontage || !AssassinationData.VictimMontage)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[Assassination Ability]: Animations not found!"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return false;
	}
	return true;
}

bool UAssassinationAbility::ValidateActivationDistance()
{
	if (FVector::DistSquared(SourceCharacter->GetActorLocation(), TargetCharacter->GetActorLocation()) > ActivationDistance * ActivationDistance)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[Assasination Ability]: Target is too far!"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return false;
	}
	return true;
}

void UAssassinationAbility::EnableMovement()
{
	if (AUmbraPlayerController* UmbraPlayerController = Cast<AUmbraPlayerController>(SourceCharacter->GetController()))
	{
		UmbraPlayerController->SwitchToDefaultContext();
	}
}
