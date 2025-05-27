// Copyrighted by Vorona Games

#include "AbilitySystem/Abilities/AssassinationAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MotionWarpingComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/Abilities/GameplayAbilitiesFunctionLibrary.h"
#include "Character/Component/InteractionComponent.h"
#include "TraversalSystem/TraversalComponent.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
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
	
	if (HasAuthority(&ActivationInfo))
	{
		RandomSeed = FMath::Rand();
	}
	else
	{
		return;
	}

	if (!InitializeAssassinationData()) return;

	if (AUmbraPlayerController* UmbraPlayerController = Cast<AUmbraPlayerController>(SourceCharacter->GetController()))
	{
		UmbraPlayerController->SwitchToCameraOnlyContext();
	}
	
	SendEventToTarget();
	StartAssassination();
}

void UAssassinationAbility::OnRep_RandomSeed()
{

	/*
	 * Если сервер запустить способность быстрее (как происхоидт в случае с Simulated Proxy)
	 * то возможно ситуация, когда инициатор и цель способности не будут инициализированы.
	 */
	if (!SourceCharacter || !TargetCharacter)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UAssassinationAbility::OnRep_RandomSeed);
		return;
	}
	
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
	AssassinationData = SourceCharacter->GetAssassinationsData()->GetRandomAssassinationDataForPositionWithSeed(
		UGameplayAbilitiesFunctionLibrary::GetActorPositionRelativeToTarget(SourceCharacter, TargetCharacter, HeightDifferenceThreshold),
		RandomSeed);
	
	if (!AssassinationData.KillerMontage || !AssassinationData.VictimMontage)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[Assasination Ability]: Animations not found!"))
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

void UAssassinationAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAssassinationAbility, RandomSeed);
}