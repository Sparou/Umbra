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
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Initiator = Cast<AUmbraPlayerCharacter>(GetAvatarActorFromActorInfo());
	
	if (!Initiator)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[Assasination Ability]: Invalid Initiator!"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	UInteractionComponent* InteractionComponent = Initiator->GetComponentByClass<UInteractionComponent>();

	if (!InteractionComponent)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[Assasination Ability]: Invalid Interaction Component!"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	Target = InteractionComponent->GetInteractionActor();

	if (!Target)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[Assasination Ability]: Invalid Target!"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (FVector::DistSquared(Initiator->GetActorLocation(), Target->GetActorLocation()) > ActivationDistance * ActivationDistance)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[Assasination Ability]: Target is too far!"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	if (HasAuthority(&ActivationInfo))
	{
		RandomSeed = FMath::Rand();
	}
	else
	{
		return;
	}
	
	AssassinationMontages = Initiator->GetStealthKillData()->GetRandomStealthKillMontagesForPositionWithSeed(
		UGameplayAbilitiesFunctionLibrary::GetActorPositionRelativeToTarget(Initiator, Target, HeightDifferenceThreshold),
		RandomSeed);

	if (!AssassinationMontages.KillerMontage || !AssassinationMontages.VictimMontage)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[Assasination Ability]: Animations not found!"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (AUmbraPlayerController* UmbraPlayerController = Cast<AUmbraPlayerController>(Initiator->GetController()))
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
	if (!Initiator || !Target)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UAssassinationAbility::OnRep_RandomSeed);
		return;
	}
	
	AssassinationMontages = Initiator->GetStealthKillData()->GetRandomStealthKillMontagesForPositionWithSeed(
		UGameplayAbilitiesFunctionLibrary::GetActorPositionRelativeToTarget(Initiator, Target, HeightDifferenceThreshold),
		RandomSeed);
	
	if (!AssassinationMontages.KillerMontage || !AssassinationMontages.VictimMontage)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[Assasination Ability]: Animations not found!"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (AUmbraPlayerController* UmbraPlayerController = Cast<AUmbraPlayerController>(Initiator->GetController()))
	{
		UmbraPlayerController->SwitchToCameraOnlyContext();
	}
	
	SendEventToTarget();
	StartAssassination();
}

void UAssassinationAbility::StartAssassination()
{
	Initiator->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	if (UMotionWarpingComponent* MotionWarping = Initiator->FindComponentByClass<UMotionWarpingComponent>())
	{
		FVector WarpLocation = Target->GetActorLocation() + AssassinationMontages.KillerStarterLocation;
		
		FVector Direction = Target->GetActorLocation() - WarpLocation;
		Direction.Z = 0;
		Direction.Normalize();
		
		FRotator WarpRotation = Direction.Rotation();
		MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("AssassinationWarp", WarpLocation, WarpRotation);
	}
	
	UAbilityTask_PlayMontageAndWait* InitiatorTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		AssassinationMontages.KillerMontage,
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
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
	{
		FGameplayTag AbilityTag = FUmbraGameplayTags::Get().Ability_Stealth_Victim;

		FGameplayEventData EventData;
		EventData.EventTag = AbilityTag;
		EventData.Instigator = GetAvatarActorFromActorInfo();
		EventData.Target = Target;
		EventData.OptionalObject = AssassinationMontages.VictimMontage;
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target, AbilityTag, EventData);
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
	Initiator->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void UAssassinationAbility::EnableMovement()
{
	if (AUmbraPlayerController* UmbraPlayerController = Cast<AUmbraPlayerController>(Initiator->GetController()))
	{
		UmbraPlayerController->SwitchToDefaultContext();
	}
}

void UAssassinationAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAssassinationAbility, RandomSeed);
}