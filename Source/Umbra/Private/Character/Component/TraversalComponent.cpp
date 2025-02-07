// Copyrighted by Vorona Games


#include "Character/Component/TraversalComponent.h"

#include "MotionWarpingComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/TraversalInterface.h"
#include "Kismet/KismetMathLibrary.h"

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

void UTraversalComponent::TriggerTraversalAction(bool JumpAction)
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
	FHitResult WallEdge;
	if (!FindWallEdge(GridWidth, GridHeight, ScanBaseLocation, ScanRotation, WallEdge))
	{
		return;
	}

	// Если состояние не "Climb", вычисляем поворот стены на основе обратного нормали
	WallRotation = ScanRotation;
	if (!TraversalState.MatchesTagExact(FUmbraGameplayTags::Get().Traversal_State_Climb))
	{
		WallRotation = ReverseNormal(WallEdge.ImpactNormal);
	}

	FHitResult WallTop;
	FHitResult LastTopHit;
	if (!FindWallTop(WallEdge, WallRotation, WallTop, LastTopHit))
	{
		return;
	}

	// Если состояние не FreeRoam, дальнейшая обработка не требуется
	if (!TraversalState.MatchesTagExact(FUmbraGameplayTags::Get().Traversal_State_FreeRoam))
	{
		return;
	}

	FHitResult WallDepth;
	if (!FindWallDepth(LastTopHit, WallRotation, WallDepth))
	{
		return;
	}

	FHitResult WallVault;
	if (!FindWallVault(WallDepth, WallRotation, WallVault))
	{
		return;
	}

	// Сохраняем результаты (при необходимости)
	WallEdgeResult = WallEdge;
	WallTopResult = WallTop;
	WallDepthResult = WallDepth;
	WallVaultResult = WallVault;
}

bool UTraversalComponent::FindWallEdge(int GridWidth, int GridHeight, const FVector& ScanBaseLocation,
	const FRotator& ScanRotation, FHitResult& OutWallEdgeResult)
{
	 TArray<FHitResult> WallHitTraces;

    for (int32 i = 0; i < GridWidth; i++)
    {
        float ColumnOffset = i * GridColumnSpacing - GridWidth * GridColumnOffsetMultiplier;
        FVector ScanColumnOrigin  = VectorDirectionMoveWithRotation(ScanBaseLocation, FUmbraGameplayTags::Get().Traversal_Direction_Right, ColumnOffset, ScanRotation);
        
        TArray<FHitResult> LineHitTraces;
        for (int32 j = 0; j < GridHeight; j++)
        {
            FVector GridPointLocation = VectorDirectionMove(ScanColumnOrigin, FUmbraGameplayTags::Get().Traversal_Direction_Up, j * GridRowSpacing);
        	FCollisionQueryParams QueryParams;
        	QueryParams.AddIgnoredActor(OwnerCharacter);
            FHitResult LineHit;
        	
        	GetWorld()->LineTraceSingleByChannel(
        		LineHit,
        		VectorDirectionMoveWithRotation(GridPointLocation, FUmbraGameplayTags::Get().Traversal_Direction_Backward, TraceDistance, ScanRotation),
        		VectorDirectionMoveWithRotation(GridPointLocation, FUmbraGameplayTags::Get().Traversal_Direction_Forward, TraceDistance, ScanRotation),
        		ECC_Visibility,
        		QueryParams);

        	LineHitTraces.Add(LineHit);
        }
    	
        for (int32 j = 1; j < LineHitTraces.Num(); j++)
        {
            FHitResult CurrentHitResult = LineHitTraces[j];
            float CurrentDistance = CurrentHitResult.bBlockingHit ? CurrentHitResult.Distance : FVector::DistSquared(CurrentHitResult.TraceStart, CurrentHitResult.TraceEnd);
            FHitResult PreviousHitResult = LineHitTraces[j - 1];
            float PreviousDistance = PreviousHitResult.bBlockingHit ? PreviousHitResult.Distance : FVector::DistSquared(PreviousHitResult.TraceStart, PreviousHitResult.TraceEnd);

            if (CurrentDistance - PreviousDistance > WallDifferenceThreshold)
            {
                WallHitTraces.Add(PreviousHitResult);
                break;
            }
        }
    }

    if (WallHitTraces.Num() <= 0)
    {
        return false;
    }
	
    OutWallEdgeResult = WallHitTraces[0];
    FVector OwnerLocation = OwnerCharacter->GetActorLocation();
    for (int32 i = 1; i < WallHitTraces.Num(); i++)
    {
        float NewDistance = FVector::DistSquared(OwnerLocation, WallHitTraces[i].ImpactPoint);
        float CurrentDistance = FVector::DistSquared(OwnerLocation, OutWallEdgeResult.ImpactPoint);
        if (NewDistance <= CurrentDistance)
        {
            OutWallEdgeResult = WallHitTraces[i];
        }
    }

	if (bDrawWallEdgeDebug)
	{
		DrawDebugSphere(GetWorld(), OutWallEdgeResult.ImpactPoint, 10.f, 8, FColor::Blue, false, 3.f);
	}
    return OutWallEdgeResult.bBlockingHit && !OutWallEdgeResult.bStartPenetrating;
}

bool UTraversalComponent::FindWallTop(const FHitResult& WallEdge, const FRotator& WallRot,
	FHitResult& OutWallTopResult, FHitResult& OutLastTopHit)
{
	bool bFoundTop = false;
	FHitResult LastTopHit;

	for (int32 i = 0; i < TopScanIterations; i++)
	{
		FVector ForwardScanLocation = VectorDirectionMoveWithRotation(WallEdge.ImpactPoint, FUmbraGameplayTags::Get().Traversal_Direction_Forward, i * TopForwardScanStep, WallRot);
		FVector ElevatedTraceStart = VectorDirectionMove(ForwardScanLocation, FUmbraGameplayTags::Get().Traversal_Direction_Up, TopElevatedOffset);
		FVector ElevatedTraceEnd = VectorDirectionMove(ElevatedTraceStart, FUmbraGameplayTags::Get().Traversal_Direction_Down, TopElevatedOffset);
		FHitResult TopHit;

		GetWorld()->SweepSingleByChannel(
			TopHit,
			ElevatedTraceStart,
			ElevatedTraceEnd,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(TopSweepSphereRadius));

		if (i == 0 && TopHit.bBlockingHit)
		{
			OutWallTopResult = TopHit;
			if (bDrawWallTopDebug)
			{
				DrawDebugSphere(GetWorld(), OutWallTopResult.ImpactPoint, 8.f, 16, FColor::Orange, false, 6.f);
			}
			bFoundTop = true;
		}
		
		if (i != 0)
		{
			if (TopHit.bBlockingHit)
			{
				LastTopHit = TopHit;
				if (bDrawWallTopDebug)
				{
					DrawDebugSphere(GetWorld(), LastTopHit.ImpactPoint, 8.f, 16, FColor::Orange, false, 6.f);
				}
			}
			else
			{
				break;
			}
		}
	}
	OutLastTopHit = LastTopHit;
	return bFoundTop;
}

bool UTraversalComponent::FindWallDepth(const FHitResult& LastTopHit, const FRotator& WallRot,
	FHitResult& OutWallDepthResult)
{
	FHitResult DepthResult;
	GetWorld()->SweepSingleByChannel(
		DepthResult,
		VectorDirectionMoveWithRotation(LastTopHit.ImpactPoint, FUmbraGameplayTags::Get().Traversal_Direction_Forward, DepthForwardOffset, WallRot),
		LastTopHit.ImpactPoint,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(DepthSweepSphereRadius));

	if (!DepthResult.bBlockingHit)
	{
		return false;
	}

	OutWallDepthResult = DepthResult;
	if (bDrawWallDepthDebug)
	{
		DrawDebugSphere(GetWorld(), OutWallDepthResult.ImpactPoint, 10.f, 8, FColor::Blue, false, 6.f);
	}
	return true;
}

bool UTraversalComponent::FindWallVault(const FHitResult& WallDepth, const FRotator& WallRot,
	FHitResult& OutWallVaultResult)
{
	FHitResult VaultHit;
	FVector Start = VectorDirectionMoveWithRotation(WallDepth.ImpactPoint, FUmbraGameplayTags::Get().Traversal_Direction_Forward, VaultForwardOffset, WallRot);
	FVector End = VectorDirectionMove(Start, FUmbraGameplayTags::Get().Traversal_Direction_Down, VaultDownOffset);

	GetWorld()->SweepSingleByChannel(
		VaultHit,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(VaultSweepSphereRadius));
	
	if (!VaultHit.bBlockingHit)
	{
		return false;
	}

	OutWallVaultResult = VaultHit;
	if (bDrawWallVaultDebug)
	{
		DrawDebugSphere(GetWorld(), OutWallVaultResult.ImpactPoint, 10.f, 8, FColor::White, false, 6.f);
	}
	return true;
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

	FRotationMatrix RotationMatrix(Rotation);
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Right))
	{
		return Source + MoveValue * RotationMatrix.GetScaledAxis(EAxis::Y);
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Left))
	{
		return Source - MoveValue * RotationMatrix.GetScaledAxis(EAxis::Y);
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Forward))
	{
		return Source + MoveValue * RotationMatrix.GetScaledAxis(EAxis::X);
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Backward))
	{
		return Source - MoveValue * RotationMatrix.GetScaledAxis(EAxis::X);
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
	int32 iterations = CharacterMovement->IsFalling() ? DetectWallIterationsFalling : DetectWallIterationsNotFalling;

	for (int32 i = 0; i < iterations; ++i)
	{
		FVector CurrentLocation = OwnerCharacter->GetActorLocation();

		FVector FirstStepVector = VectorDirectionMove(CurrentLocation, UGT.Traversal_Direction_Down, DetectWallDownOffset);
		FVector SecondStepVector = VectorDirectionMove(FirstStepVector, UGT.Traversal_Direction_Up, i * DetectWallUpStepMultiplier);
		FVector ThirdStepVector = VectorDirectionMove(SecondStepVector, UGT.Traversal_Direction_Backward, DetectWallBackwardOffset);
		FVector FourthStepVector = VectorDirectionMove(SecondStepVector, UGT.Traversal_Direction_Forward, DetectWallForwardOffset);
		
		if (GetWorld()->SweepSingleByChannel(
			HitResult,
			ThirdStepVector,
			FourthStepVector,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(DetectWallSphereRadius)))
		{
			if (HitResult.bBlockingHit && !HitResult.bStartPenetrating)
			{
				if (bDrawDetectWallDebug)
				{
					DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 8.f, 8, FColor::Green, false, 3.f);
				}
				return HitResult;
			}
		}
	}

	return HitResult;
}





