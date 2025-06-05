// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/GameplayAbilitiesFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "UmbraGameplayTags.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

FGameplayTag UGameplayAbilitiesFunctionLibrary::GetActorPositionRelativeToTarget(const AActor* Actor,
                                                                                 const AActor* Target, float HeightDifferenceThreshold)
{
	FUmbraGameplayTags UGT = FUmbraGameplayTags::Get();
  
	FVector ActorLocation = Actor->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	float HeightDifference = ActorLocation.Z - TargetLocation.Z;
	if (HeightDifference > HeightDifferenceThreshold)
	{
		return UGT.Position_Top;
	}
  
	FVector TargetForwardVector = Target->GetActorForwardVector();
	FVector TargetRightVector = Target->GetActorRightVector();
	FVector Direction = ActorLocation - TargetLocation;

	float Distance = Direction.Size();
	Direction.Normalize();

	// Убийца находится спереди или сзади?
	float ForwardDot = FVector::DotProduct(TargetForwardVector, Direction);
	float RightDot = FVector::DotProduct(TargetRightVector, Direction);
	UE_LOG(LogTemp, Warning, TEXT("ForwardDot: %f, RightDot: %f"), ForwardDot, RightDot);

	if (ForwardDot > 0.5f) // Угол < 60° — игрок находится спереди
	{
		return UGT.Position_Front;
	}
	if (ForwardDot < -0.5f) // Угол > 120° — игрок находится сзади
	{
		return UGT.Position_Behind;
	}

	// Убийца находится слева или справа?
	if (RightDot > 0.5f) // Угол < 60° — справа
	{
		return UGT.Position_Right;
	}
	if (RightDot < -0.5f) // Угол > 120° — слева
	{
		return UGT.Position_Left;
	}

	return UGT.Position_Behind;
}

FVector UGameplayAbilitiesFunctionLibrary::GetTargetLocation(UGameplayAbility* Ability, AActor* Actor, FName BlackboardVariableName, FName BoneName)
{
	ACharacter* Character  = Cast<ACharacter>(Actor);
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
	
	if (!Character)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[%s] isn't Character!"), *GetNameSafe(Character));
		ASC->CancelAbility(Ability);
	}

	if (Character->IsPlayerControlled())
	{
		FVector TargetLocation;
		FRotator TargetRotation;
		Character->GetController()->GetPlayerViewPoint(TargetLocation, TargetRotation);

		const FVector ViewDirection = TargetRotation.Vector();
		return TargetLocation + ViewDirection * 1000;
	}

	AAIController* AIController = Cast<AAIController>(Character->GetController());
	if (!AIController)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[%s] isn't controlled by AIController!"), *GetNameSafe(Character));
		ASC->CancelAbility(Ability);
	}

	UBlackboardComponent* BlackBoard = AIController->GetBlackboardComponent();
	if (!BlackBoard)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[%s] has invalid BlackboardComponent!"), *GetNameSafe(Character));
		ASC->CancelAbility(Ability);
	}

	ACharacter* AITarget = Cast<ACharacter>(BlackBoard->GetValueAsObject(BlackboardVariableName));
	if (!AITarget)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[%s] has no valid value of [%s] in Blackboard!"), *BlackboardVariableName.ToString(), *GetNameSafe(Character));
		ASC->CancelAbility(Ability);
	}

	if (AITarget->GetMesh())
	{
		return AITarget->GetMesh()->GetBoneLocation(BoneName);
	}

	return AITarget->GetActorLocation();
}

FRotator UGameplayAbilitiesFunctionLibrary::GetRotationToTarget(FVector CurrentLocation, FVector TargetLocation)
{
	FVector Direction = (TargetLocation -  CurrentLocation);
	Direction.Normalize();
	Direction.Z = 0;
	return Direction.Rotation();
}
