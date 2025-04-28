// Copyrighted by Vorona Games

#include "AbilitySystem/Abilities/Stealth/UmbraStealthKillAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "UmbraGameplayTags.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character//Component/InteractionComponent.h"
#include "UmbraCoreTypes.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/UmbraPlayerController.h"

void UUmbraStealthKillAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	AvatarCharacter = Cast<AUmbraPlayerCharacter>(GetAvatarActorFromActorInfo());
	UInteractionComponent* InteractionComponent = AvatarCharacter->GetComponentByClass<UInteractionComponent>();
	TargetActor = InteractionComponent->GetCurrentTarget();
	
	if (!TargetActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Stealth Kill Ability: No target actor found!"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	switch (const EStealthKillPosition KillPosition = CheckAvatarActorPosition())
	{
	case EStealthKillPosition::Behind:
		StealthKillMontages = AvatarCharacter->GetStealthKillMontageForPosition(EStealthKillPosition::Behind);
		break;
	
	case EStealthKillPosition::Front:
		StealthKillMontages = AvatarCharacter->GetStealthKillMontageForPosition(EStealthKillPosition::Front);
		break;
	
	case EStealthKillPosition::Top:
		StealthKillMontages = AvatarCharacter->GetStealthKillMontageForPosition(EStealthKillPosition::Top);
		break;
	
	case EStealthKillPosition::Ledge:
		StealthKillMontages = AvatarCharacter->GetStealthKillMontageForPosition(EStealthKillPosition::Ledge);
		break;
	
	case EStealthKillPosition::Left:
		StealthKillMontages = AvatarCharacter->GetStealthKillMontageForPosition(EStealthKillPosition::Left);
		break;
	
	case EStealthKillPosition::Right:
		StealthKillMontages = AvatarCharacter->GetStealthKillMontageForPosition(EStealthKillPosition::Right);
		break;
	
	default:
		UE_LOG(LogTemp, Error, TEXT("Unknown KillPosition: %d"), static_cast<int32>(KillPosition));
		break;
	}

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

	TimerCallback = [this](){MoveToKillPosition();};
	RotateCharacterToTarget(TargetActor->GetActorLocation() - StealthKillMontages.KillerStarterLocation, 0.008f);
	
}


EStealthKillPosition UUmbraStealthKillAbility::CheckAvatarActorPosition() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	
	if (!AvatarActor || !TargetActor)
	{
		return EStealthKillPosition::Behind;
	}
	
	FVector AvatarLocation = AvatarActor->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();

	float HeightDifference = AvatarLocation.Z - TargetLocation.Z;
	if (HeightDifference > HeightDifferenceThreshold)
	{
		return EStealthKillPosition::Top;
	}
	
	FVector TargetForwardVector = TargetActor->GetActorForwardVector();
	FVector TargetRightVector = TargetActor->GetActorRightVector();
	FVector Direction = AvatarLocation - TargetLocation;

	float Distance = Direction.Size();
	Direction.Normalize();

	// Убийца находится спереди или сзади?
	float ForwardDot = FVector::DotProduct(TargetForwardVector, Direction);
	float RightDot = FVector::DotProduct(TargetRightVector, Direction);
	UE_LOG(LogTemp, Warning, TEXT("ForwardDot: %f, RightDot: %f"), ForwardDot, RightDot);

	if (ForwardDot > 0.5f) // Угол < 60° — игрок находится спереди
	{
		return EStealthKillPosition::Front;
	}
	else if (ForwardDot < -0.5f) // Угол > 120° — игрок находится сзади
	{
		return EStealthKillPosition::Behind;
	}

	// Убийца находится слева или справа?
	if (RightDot > 0.5f) // Угол < 60° — справа
	{
		return EStealthKillPosition::Right;
	}
	else if (RightDot < -0.5f) // Угол > 120° — слева
	{
		return EStealthKillPosition::Left;
	}

	return EStealthKillPosition::Behind;
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
	
	float Distance = (AvatarCharacter->GetActorLocation() - TargetActor->GetActorLocation()).Size();
	//DrawDebugSphere(GetWorld(), TargetActor->GetActorLocation() + StealthKillMontages.KillerStarterLocation, 16, 32, FColor::Red, false, 5.f);
	
	if(UAbilityTask_ApplyRootMotionMoveToForce* MoveToForce = UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
		this,
		NAME_None,
		TargetActor->GetActorLocation() + StealthKillMontages.KillerStarterLocation,
		Distance / AvatarCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched,
		false,
		EMovementMode::MOVE_Walking,
		false,
		nullptr,
		ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,
		FVector::ZeroVector,
		0.0f))
	{
		MoveToForce->OnTimedOutAndDestinationReached.AddDynamic(this, &UUmbraStealthKillAbility::OnMoveCompleted);
		MoveToForce->OnTimedOut.AddDynamic(this, &UUmbraStealthKillAbility::OnMoveCompleted);
		MoveToForce->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Stealth Kill Ability: MoveToTask failed!"));
		EnableMovement();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}


void UUmbraStealthKillAbility::OnMoveCompleted()
{
	TimerCallback = [this](){StartStealthKill();};
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
	
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		FGameplayTag AbilityTag = FUmbraGameplayTags::Get().Ability_Stealth_Victim;

		FGameplayEventData EventData;
		EventData.EventTag = AbilityTag;
		EventData.Instigator = GetAvatarActorFromActorInfo();
		EventData.Target = TargetActor;
		EventData.OptionalObject = StealthKillMontages.VictimMontage;
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, AbilityTag, EventData);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Stealth Kill Ability: Target actor doesn't have ability system component!"));
		EnableMovement();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		StealthKillMontages.KillerMontage,
		1.0f,
		NAME_None,
		false,
		1.0f);

	PlayMontageTask->OnCompleted.AddDynamic(this, &UUmbraStealthKillAbility::OnMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &UUmbraStealthKillAbility::OnMontageInterrupted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &UUmbraStealthKillAbility::OnMontageCancelled);
	PlayMontageTask->ReadyForActivation();
}


void UUmbraStealthKillAbility::OnMontageCompleted()
{
	EnableMovement();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	UE_LOG(LogTemp, Warning, TEXT("Stealth kill completed"));
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
}

