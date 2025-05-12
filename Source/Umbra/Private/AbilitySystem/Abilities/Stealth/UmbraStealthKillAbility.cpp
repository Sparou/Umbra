// Copyrighted by Vorona Games

#include "AbilitySystem/Abilities/Stealth/UmbraStealthKillAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "UmbraGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToActorForce.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "AbilitySystem/Abilities/GameplayAbilitiesFunctionLibrary.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Character//Component/InteractionComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/UmbraPlayerController.h"
#include "TraversalSystem/TraversalComponent.h"

void UUmbraStealthKillAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Log, TEXT("Kill ablitiy started!"));
	
	AvatarCharacter = Cast<AUmbraPlayerCharacter>(GetAvatarActorFromActorInfo());
	UInteractionComponent* InteractionComponent = AvatarCharacter->GetComponentByClass<UInteractionComponent>();

	if (!InteractionComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Interaction Component is null!"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	TargetActor = InteractionComponent->GetInteractionActor();
	
	if (!TargetActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Stealth Kill Ability: No target actor found!"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	StealthKillMontages = AvatarCharacter->GetStealthKillData()->GetRandomStealthKillMontagesForPosition(
		UGameplayAbilitiesFunctionLibrary::GetActorPositionRelativeToTarget(
			AvatarCharacter,
			TargetActor,
			HeightDifferenceThreshold));
	
	if (!StealthKillMontages.KillerMontage || !StealthKillMontages.VictimMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("Stealth Kill Ability: No animation found!"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (AUmbraPlayerController* UmbraPlayerController = Cast<AUmbraPlayerController>(AvatarCharacter->GetController()))
	{
		UmbraPlayerController->SwitchToCameraOnlyContext();
	}

	AvatarCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
	TimerCallback = [this]()
	{
		UAbilityTask_NetworkSyncPoint* SyncTask =
			UAbilityTask_NetworkSyncPoint::WaitNetSync(this, /*SyncType=*/EAbilityTaskNetSyncType::BothWait);
		SyncTask->OnSync.AddDynamic(this, &UUmbraStealthKillAbility::MoveToKillPosition);
		SyncTask->ReadyForActivation();
	};
	RotateCharacterToTarget(TargetActor->GetActorLocation() - StealthKillMontages.KillerStarterLocation, 0.008f);
	
}

void UUmbraStealthKillAbility::RotateCharacterToTarget(const FVector& TargetLocation, float RotationInRate)
{
	if (USpringArmComponent* SpringArm = AvatarCharacter->FindComponentByClass<USpringArmComponent>())
	{
		CameraOffset = SpringArm->GetRelativeTransform().GetLocation();
		SpringArm->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
	
	// Вычисляем направление на цель
	FVector Direction = TargetLocation - AvatarCharacter->GetActorLocation();
	Direction.Z = 0;
	Direction.Normalize();
	
	FRotator TargetRotation = Direction.Rotation();
	DesiredRotation = TargetRotation;
	
	FVector WorldOffset = DesiredRotation.RotateVector(CameraOffset);
	CameraDesiredLocation = AvatarCharacter->GetActorLocation() + WorldOffset;
	
	GetAvatarActorFromActorInfo()->GetWorld()->GetTimerManager().SetTimer(
		RotationTimerHandle,
		this,
		&UUmbraStealthKillAbility::UpdateRotation,
		RotationInRate,
		true);
}

void UUmbraStealthKillAbility::UpdateRotation()
{
	FRotator CurrentRotation = AvatarCharacter->GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, GetWorld()->GetDeltaSeconds(), 5.0f);
	AvatarCharacter->SetActorRotation(NewRotation);

	if (USpringArmComponent* SpringArm = AvatarCharacter->FindComponentByClass<USpringArmComponent>())
	{
		FVector CurrentCameraLocation = SpringArm->GetComponentTransform().GetLocation();
		FVector NewLocation = FMath::VInterpTo(CurrentCameraLocation, CameraDesiredLocation, GetWorld()->GetDeltaSeconds(), 5.0f);
		SpringArm->SetWorldLocation(NewLocation);
	}
	
	if (CurrentRotation.Equals(DesiredRotation, 2.0f))
	{
		GetWorld()->GetTimerManager().ClearTimer(RotationTimerHandle);
		TimerCallback();
	}
}


void UUmbraStealthKillAbility::MoveToKillPosition()
{
	if (USpringArmComponent* SpringArm = AvatarCharacter->FindComponentByClass<USpringArmComponent>())
	{
		SpringArm->AttachToComponent(
			AvatarCharacter->GetRootComponent(), 
			FAttachmentTransformRules::KeepWorldTransform
		);
	}

	TraversalComponent = AvatarCharacter->GetComponentByClass<UTraversalComponent>();
	if (TraversalComponent)
	{
		TraversalComponent->SetComponentTickEnabled(false);
	}
	
	float Distance = (AvatarCharacter->GetActorLocation() - TargetActor->GetActorLocation()).Size();
	
	AdditionalVerticalOffset = AvatarCharacter->bIsCrouched ?
		FVector(0, 0, -(AvatarCharacter->GetDefaultHalfHeight() - AvatarCharacter->GetCharacterMovement()->GetCrouchedHalfHeight())) :
		FVector::ZeroVector;

	MaxSpeed = AvatarCharacter->bIsCrouched ?
		AvatarCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched :
		AvatarCharacter->GetCharacterMovement()->MaxWalkSpeed;
	
	UE_LOG(UmbraAbilitiesLog, Log, TEXT("Time = [%f]"), GetWorld()->GetTimeSeconds());
	if(UAbilityTask_ApplyRootMotionMoveToActorForce* MoveToActorForce = UAbilityTask_ApplyRootMotionMoveToActorForce::ApplyRootMotionMoveToActorForce(
	this,
	NAME_None,
	TargetActor,
	StealthKillMontages.KillerStarterLocation + AdditionalVerticalOffset + FVector(0, 0, -30),
	ERootMotionMoveToActorTargetOffsetType::AlignToTargetForward,
	Distance / MaxSpeed,
	nullptr,
	nullptr,
	false,
	MOVE_Walking,
	false,
	nullptr,
	nullptr,
	ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,
	FVector::ZeroVector,
	0.0f,
	false,
	10.f))
	{
		MoveToActorForce->OnFinished.AddDynamic(this, &UUmbraStealthKillAbility::OnMoveCompleted);
		MoveToActorForce->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Stealth Kill Ability: MoveToTask failed!"));
		EnableMovement();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}


void UUmbraStealthKillAbility::OnMoveCompleted(bool bTimedOut, bool bReachedDestination, FVector FinalTargetLocation)
{
	TimerCallback = [this]()
	{
		UAbilityTask_NetworkSyncPoint* FinishSync =
			UAbilityTask_NetworkSyncPoint::WaitNetSync(this, /*SyncType=*/EAbilityTaskNetSyncType::BothWait);
		FinishSync->OnSync.AddDynamic(this, &UUmbraStealthKillAbility::StartStealthKill);
		FinishSync->ReadyForActivation();
	};
	RotateCharacterToTarget(TargetActor->GetActorLocation(), 0.008f);
}


void UUmbraStealthKillAbility::StartStealthKill()
{
	if (USpringArmComponent* SpringArm = AvatarCharacter->FindComponentByClass<USpringArmComponent>())
	{
		SpringArm->AttachToComponent(
			AvatarCharacter->GetRootComponent(), 
			FAttachmentTransformRules::KeepWorldTransform
		);
	}

	SendEventToTarget();

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		StealthKillMontages.KillerMontage,
		1.f,
		NAME_None,
		true,
		1.0f);
	
	PlayMontageTask->OnCompleted.AddDynamic(this, &UUmbraStealthKillAbility::OnMontageCompleted);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &UUmbraStealthKillAbility::OnMontageBlendingOut);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &UUmbraStealthKillAbility::OnMontageInterrupted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &UUmbraStealthKillAbility::OnMontageCancelled);
	PlayMontageTask->ReadyForActivation();
}

void UUmbraStealthKillAbility::SendEventToTarget()
{
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		FGameplayTag AbilityTag = FUmbraGameplayTags::Get().Ability_Stealth_Victim;

		FGameplayEventData EventData;
		EventData.EventTag = AbilityTag;
		EventData.Instigator = GetAvatarActorFromActorInfo();
		EventData.Target = TargetActor;
		EventData.OptionalObject = StealthKillMontages.VictimMontage;
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, AbilityTag, EventData);
		UE_LOG(LogTemp, Log, TEXT("Event was sent!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Stealth Kill Ability: Target actor doesn't have ability system component!"));
		EnableMovement();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}


void UUmbraStealthKillAbility::OnMontageCompleted()
{
	if (AvatarCharacter->IsLocallyControlled())
	{
		AvatarCharacter->GetCharacterMovement()->NetworkSmoothingMode = ENetworkSmoothingMode::Linear; // или Exponential, как у вас по умолчанию
	}
	
	EnableMovement();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	UE_LOG(LogTemp, Warning, TEXT("Stealth kill completed"));
}

void UUmbraStealthKillAbility::OnMontageBlendingOut()
{
	AvatarCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void UUmbraStealthKillAbility::OnMontageInterrupted()
{
	EnableMovement();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	UE_LOG(LogTemp, Warning, TEXT("Stealth kill interrupted"));
}

void UUmbraStealthKillAbility::OnMontageCancelled()
{
	EnableMovement();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	UE_LOG(LogTemp, Warning, TEXT("Stealth kill cancelled"));
}

void UUmbraStealthKillAbility::EnableMovement() const
{
	if (AUmbraPlayerController* UmbraPlayerController = Cast<AUmbraPlayerController>(AvatarCharacter->GetController()))
	{
		UmbraPlayerController->SwitchToDefaultContext();
	}

	if (TraversalComponent)
	{
		TraversalComponent->SetComponentTickEnabled(true);
	}
}

