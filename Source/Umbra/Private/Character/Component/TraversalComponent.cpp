// Copyrighted by Vorona Games


#include "Character/Component/TraversalComponent.h"

#include "MotionWarpingComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/TraversalInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UTraversalComponent::UTraversalComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UTraversalComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeReferences();
}

void UTraversalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTraversalComponent::TriggerTraversalAction(const bool& JumpAction)
{
	if (TraversalAction.MatchesTagExact(FUmbraGameplayTags::Get().Traversal_Action_NoAction))
	{
		FHitResult HitResult = DetectWall();
		if (!HitResult.bBlockingHit)
		{
			OwnerCharacter->Jump();
			return;
		}

		GridScan(4, 30, HitResult.ImpactPoint, ReverseNormal(HitResult.ImpactNormal));
	}
}

void UTraversalComponent::GridScan(int GridWidth, int GridHeight, const FVector& ScanBaseLocation,
	const FRotator& ScanRotation)
{
	TArray<FHitResult> WallHitTraces = TArray<FHitResult>();
	TArray<FHitResult> LineHitTraces = TArray<FHitResult>();

	WallHitTraces.Empty();

	for (size_t i = 0; i < GridWidth; i++)
	{
		float MoveValue = i * 20.f - GridWidth * 10.f;
		FVector First = VectorDirectionMoveWithRotation(ScanBaseLocation, FUmbraGameplayTags::Get().Traversal_Direction_Right, MoveValue, ScanRotation);
		LineHitTraces.Empty();
		for (size_t j = 0; j < GridHeight; j++)
		{
			FVector Second = VectorDirectionMove(First, FUmbraGameplayTags::Get().Traversal_Direction_Up, j * 8);
			FHitResult LineHit;
			UKismetSystemLibrary::LineTraceSingle(
				this,
				VectorDirectionMoveWithRotation(Second, FUmbraGameplayTags::Get().Traversal_Direction_Backward, 60.f, ScanRotation),
				VectorDirectionMoveWithRotation(Second, FUmbraGameplayTags::Get().Traversal_Direction_Forward, 60.f, ScanRotation),
				UEngineTypes::ConvertToTraceType(ECC_Visibility),
				false,
				TArray<AActor*>(),
				EDrawDebugTrace::ForDuration,
				LineHit,
				true,
				FLinearColor::Red,
				FLinearColor::Green,
				5.f);
			LineHitTraces.Add(LineHit);
		}

		 for (size_t j = 1; j < LineHitTraces.Num(); j++)
		 {
		 	FHitResult CurrentHitResult = LineHitTraces[j];
		 	float CurrentDistance = CurrentHitResult.bBlockingHit ? CurrentHitResult.Distance : UKismetMathLibrary::Vector_Distance(CurrentHitResult.TraceStart, CurrentHitResult.TraceEnd);
		 	FHitResult PreviousHitResult = LineHitTraces[j-1];
		 	float PreviousDistance = PreviousHitResult.bBlockingHit ? PreviousHitResult.Distance : UKismetMathLibrary::Vector_Distance(PreviousHitResult.TraceStart, PreviousHitResult.TraceEnd);

		 	if (CurrentDistance - PreviousDistance > 5)
		 	{
		 		WallHitTraces.Add(PreviousHitResult);
		 		break;
		 	}
		 }
	}
	
	if (WallHitTraces.Num() <= 0) return;

	WallEdgeResult = WallHitTraces[0];
	FVector OwnerLocation = OwnerCharacter->GetActorLocation();

	for (size_t i = 1; i < WallHitTraces.Num(); i++)
	{
		float NewDistance = UKismetMathLibrary::Vector_Distance(OwnerLocation, WallHitTraces[i].ImpactPoint);
		float CurrentDistance = UKismetMathLibrary::Vector_Distance(OwnerLocation, WallEdgeResult.ImpactPoint);
		if (NewDistance <= CurrentDistance)
		{
			WallEdgeResult = WallHitTraces[i];
		}
	}

	DrawDebugSphere(GetWorld(), WallEdgeResult.ImpactPoint, 10.f, 8, FColor::Blue, false, 3.f);

	if (!WallEdgeResult.bBlockingHit || WallEdgeResult.bStartPenetrating) return;
	if (!TraversalState.MatchesTagExact(FUmbraGameplayTags::Get().Traversal_State_Climb))
	{
		WallRotation = ReverseNormal(WallEdgeResult.ImpactNormal);
	}
	FHitResult LastTopHit = FHitResult();

	for (size_t i = 0; i < 8; i++)
	{
		FVector First = VectorDirectionMoveWithRotation(WallEdgeResult.ImpactPoint, FUmbraGameplayTags::Get().Traversal_Direction_Forward, i * 30, WallRotation);
		FVector Second = VectorDirectionMove(First, FUmbraGameplayTags::Get().Traversal_Direction_Up, 7.f);
		FVector Third = VectorDirectionMove(Second, FUmbraGameplayTags::Get().Traversal_Direction_Down, 7.f);
		FHitResult TopHit;

		bool bHit = UKismetSystemLibrary::SphereTraceSingle(
			this,
			Second,
			Third,
			2.5f,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForDuration,
			TopHit,
			true,
			FLinearColor::Green,
			FLinearColor::White,
			5);

		if (i == 0 && bHit)
		{
			WallTopResult = TopHit;
			DrawDebugSphere(GetWorld(), WallTopResult.ImpactPoint, 8.f, 16, FColor::Orange, false, 6.f);
		}

		if (i != 0)
		{
			if (bHit)
			{
				LastTopHit = TopHit;
				DrawDebugSphere(GetWorld(), LastTopHit.ImpactPoint, 8.f, 16, FColor::Orange, false, 6.f);
			}
			else
			{
				break;
			}
		}
	}

	if (!TraversalState.MatchesTagExact(UGT.Traversal_State_FreeRoam))
	{
		return;
	}

	FHitResult DepthResult = FHitResult();
	bool bHit = UKismetSystemLibrary::SphereTraceSingle(
	this,
	VectorDirectionMoveWithRotation(LastTopHit.ImpactPoint, UGT.Traversal_Direction_Forward, 30.f, WallRotation),
	LastTopHit.ImpactPoint,
	10.f,
	UEngineTypes::ConvertToTraceType(ECC_Visibility),
	false,
	TArray<AActor*>(),
	EDrawDebugTrace::None,
	DepthResult,
	true,
	FLinearColor::Green,
	FLinearColor::White,
	5);

	if (!bHit) return;
	
	WallDepthResult = DepthResult;
	DrawDebugSphere(GetWorld(), WallDepthResult.ImpactPoint, 10.f, 8, FColor::Blue, false, 6.f);
	
	FHitResult Vault = FHitResult();
	FVector Start = VectorDirectionMoveWithRotation(WallDepthResult.ImpactPoint, UGT.Traversal_Direction_Forward, 70.f, WallRotation);
	FVector End = VectorDirectionMove(Start, UGT.Traversal_Direction_Down, 200.f);
	bHit = UKismetSystemLibrary::SphereTraceSingle(
	this,
	Start,
	End,
	10.f,
	UEngineTypes::ConvertToTraceType(ECC_Visibility),
	false,
	TArray<AActor*>(),
	EDrawDebugTrace::None,
	Vault,
	true,
	FLinearColor::Green,
	FLinearColor::White,
	5);

	if (!bHit) return;

	WallVaultResult = Vault;
	DrawDebugSphere(GetWorld(), WallVaultResult.ImpactPoint, 10.f, 8, FColor::White, false, 6.f);
}

void UTraversalComponent::InitializeReferences()
{
	OwnerCharacter = Cast<ACharacter>(this->GetOwner());
	CharacterMovement = OwnerCharacter->GetCharacterMovement();
	MotionWarping = OwnerCharacter->FindComponentByClass<UMotionWarpingComponent>();
	Camera = OwnerCharacter->FindComponentByClass<UCameraComponent>();
	Capsule = OwnerCharacter->FindComponentByClass<UCapsuleComponent>();
	SkeletalMesh = OwnerCharacter->GetMesh();
	AnimInstance = SkeletalMesh->GetAnimInstance();

	check(OwnerCharacter);
	check(CharacterMovement);
	check(MotionWarping);
	check(Camera);
	check(Capsule);
	check(SkeletalMesh);
	check(AnimInstance);
}

void UTraversalComponent::SetTraversalStateSettings(ECollisionEnabled::Type CollisionType, EMovementMode MovementMode,
	const bool& StopMovementImmediately)
{
	Capsule->SetCollisionEnabled(CollisionType);
	CharacterMovement->SetMovementMode(MovementMode);
	if (StopMovementImmediately)
	{
		CharacterMovement->StopMovementImmediately();
	}
}

void UTraversalComponent::SetTraversalState(const FGameplayTag& NewTraversalState)
{
	if (NewTraversalState.MatchesTagExact(TraversalState))
	{
		return;
	}

	TraversalState = NewTraversalState;
	
	if (ITraversalInterface* TraversalAnimInstance = Cast<ITraversalInterface>(AnimInstance))
	{
		TraversalAnimInstance->SetTraversalState(NewTraversalState);
	}

	//TODO: По идеи это можно сжать, но я не уверен, не понадобится в телах разная лоигка.
	//TODO: Оптимизирвать после доведения системы до рабочего состояния.
	if (NewTraversalState.MatchesTagExact(UGT.Traversal_State_FreeRoam))
	{
		SetTraversalStateSettings(ECollisionEnabled::Type::QueryAndPhysics, MOVE_Walking, false);
	}
	else if (NewTraversalState.MatchesTagExact(UGT.Traversal_State_ReadyToClimb))
	{
		SetTraversalStateSettings(ECollisionEnabled::Type::NoCollision, MOVE_Flying, false);
	}
	else if (NewTraversalState.MatchesTagExact(UGT.Traversal_State_Climb))
	{
		SetTraversalStateSettings(ECollisionEnabled::Type::NoCollision, MOVE_Flying, true);
	}
	else if (NewTraversalState.MatchesTagExact(UGT.Traversal_State_Mantle))
	{
		SetTraversalStateSettings(ECollisionEnabled::Type::NoCollision, MOVE_Flying, false);
	}
	else if (NewTraversalState.MatchesTagExact(UGT.Traversal_State_Vault))
	{
		SetTraversalStateSettings(ECollisionEnabled::Type::NoCollision, MOVE_Flying, false);
	}
	
}

void UTraversalComponent::SetClimbStyle(const FGameplayTag& NewClimbStyle)
{
	if (NewClimbStyle.MatchesTagExact(ClimbStyle))
	{
		return;
	}

	ClimbStyle = NewClimbStyle;

	if (ITraversalInterface* TraversalAnimInstance = Cast<ITraversalInterface>(AnimInstance))
	{
		TraversalAnimInstance->SetClimbStyle(NewClimbStyle);
	}
}

void UTraversalComponent::SetClimbDirection(const FGameplayTag& NewClimbDirection)
{
	if (NewClimbDirection.MatchesTagExact(ClimbDirection))
	{
		return;
	}

	ClimbDirection = NewClimbDirection;

	if (ITraversalInterface* TraversalAnimInstance = Cast<ITraversalInterface>(AnimInstance))
	{
		TraversalAnimInstance->SetClimbDirection(NewClimbDirection);
	}
}

float UTraversalComponent::ClimbValues(const FGameplayTag& NewClimbStyle, const float& Braced, const float& FreeHang) const
{
	return ClimbStyle.MatchesTagExact(FUmbraGameplayTags::Get().Traversal_ClimbStyle_BracedClimb) ? Braced : FreeHang;
}

FVector UTraversalComponent::VectorDirectionMove(const FVector& Source, const FGameplayTag& Direction, const float& MoveValue)
{
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Up))
	{
		return Source + FVector(0.f, 0.f, MoveValue);
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Down))
	{
		return Source - FVector(0.f, 0.f, MoveValue);
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Right))
	{
		return Source + MoveValue * OwnerCharacter->GetActorRightVector();
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Left))
	{
		return Source - MoveValue * OwnerCharacter->GetActorRightVector();
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Forward))
	{
		return Source + MoveValue * OwnerCharacter->GetActorForwardVector();
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Backward))
	{
		return Source - MoveValue * OwnerCharacter->GetActorForwardVector();
	}
	
	return Source;
}

FVector UTraversalComponent::VectorDirectionMoveWithRotation(const FVector& Source, const FGameplayTag& Direction,
	const float& MoveValue, const FRotator& Rotation)
{
	//UE_LOG(LogTemp, Warning, TEXT("Direction: [%s], Value: [%f]"), *Direction.GetTagName().ToString(), MoveValue);
	
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Right))
	{
		return Source + MoveValue * FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Left))
	{
		return Source - MoveValue * FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Forward))
	{
		return Source + MoveValue * FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Backward))
	{
		return Source - MoveValue * FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
	}
	return Source;
}

FRotator UTraversalComponent::ReverseNormal(const FVector& Normal)
{
	FRotator Rotator = UKismetMathLibrary::MakeRotFromX(Normal);
	FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(Rotator, FRotator(0.f, 180.f, 0.f ));
	return FRotator(0.f, DeltaRotator.Yaw, 0.f);
}

FHitResult UTraversalComponent::DetectWall()
{
	FHitResult HitResult;
	size_t iterations = CharacterMovement->IsFalling() ? 8 : 15;

	for (size_t i = 0; i < iterations; ++i)
	{
		FVector CurrentLocation = OwnerCharacter->GetActorLocation();

		FVector FirstStepVector = VectorDirectionMove(CurrentLocation, UGT.Traversal_Direction_Down, 60.f);
		FVector SecondStepVector = VectorDirectionMove(FirstStepVector, UGT.Traversal_Direction_Up, i * 16.f);
		FVector ThirdStepVector = VectorDirectionMove(SecondStepVector, UGT.Traversal_Direction_Backward, 20.f);
		FVector FourthStepVector = VectorDirectionMove(SecondStepVector, UGT.Traversal_Direction_Forward, 140.f);
		
		if (GetWorld()->SweepSingleByChannel(
			HitResult,
			ThirdStepVector,
			FourthStepVector,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(10.f)))
		{
			if (HitResult.bBlockingHit && !HitResult.bStartPenetrating)
			{
#if WITH_EDITOR
				DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 8.f, 4, FColor::Green, false, 3.f);
#endif
				return HitResult;
			}
		}
	}

	return HitResult;
}

FHitResult UTraversalComponent::FindWallEdge(int GridWidth, int GridHeight, const FVector& ScanBaseLocation,
	const FRotator& ScanRotation)
{
	return FHitResult();
}




