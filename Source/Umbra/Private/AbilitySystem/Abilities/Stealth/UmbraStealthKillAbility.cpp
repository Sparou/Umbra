// Copyrighted by Vorona Games

#include "AbilitySystem/Abilities/Stealth/UmbraStealthKillAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "UmbraGameplayTags.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UUmbraStealthKillAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	AUmbraPlayerCharacter* AvatarCharacter = Cast<AUmbraPlayerCharacter>(GetAvatarActorFromActorInfo());
	TargetActor  = AvatarCharacter->GetOverlappedActorFromInteractionComponent();
	
	if (!TargetActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Stealth Kill Ability: No target actor found!"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	if (!CheckAvatarActorPosition())
	{
		UE_LOG(LogTemp, Error, TEXT("Avatar Actor has wrong postition. He must be behind the target!"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
	FTransform StealthKillStartPoint = TargetCharacter->GetMesh()->GetSocketTransform("StealthKillSocket");
	
	StealthKillMontages = AvatarCharacter->GetRandomStealthKillMontages();
	
	if (APlayerController* AvatarController = Cast<APlayerController>(AvatarCharacter->GetController()))
	{
		AvatarController->DisableInput(AvatarController);
	}

	AvatarCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	AvatarCharacter->GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	if(UAbilityTask_ApplyRootMotionMoveToForce* MoveToForce = UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
		this,
		NAME_None,
		StealthKillStartPoint.GetLocation(),
		5.0f,
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
	FVector TargetLocation = TargetActor->GetActorLocation();
	FVector CharacterLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CharacterLocation, TargetLocation);
	
	AUmbraPlayerCharacter* AvatarCharacter = Cast<AUmbraPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (APlayerController* AvatarController = Cast<APlayerController>(AvatarCharacter->GetController()))
	{
		AvatarController->SetControlRotation(LookAtRotation);
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
	AUmbraPlayerCharacter* AvatarCharacter = Cast<AUmbraPlayerCharacter>(GetAvatarActorFromActorInfo());
	AvatarCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	AvatarCharacter->GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	APlayerController* AvatarController = Cast<APlayerController>(AvatarCharacter->GetController());
	AvatarController->EnableInput(AvatarController);
}

EStealthKillPosition UUmbraStealthKillAbility::CheckAvatarActorPosition() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	FVector TargetForwardVector = TargetActor->GetActorForwardVector();
	FVector Direction = AvatarActor->GetActorLocation() - TargetActor->GetActorLocation();
	float Distance = Direction.Size();
	Direction.Normalize();

	float DotProduct = FVector::DotProduct(TargetForwardVector, Direction);
	
	return DotProduct < -0.7f;
}
