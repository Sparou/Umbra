// Copyrighted by Vorona Games


#include "TraversalSystem/TraversalComponent.h"

#include "KismetAnimationLibrary.h"
#include "MotionWarpingComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TraversalSystem/TraversalInterface.h"
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

	ValidateIsInLand();
	if (bInLand)
	{
		if (TraversalAction.MatchesTagExact(UGT.Traversal_Action_NoAction))
		{
			ResetTraversalResults();
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Traversal System: Character is in air!"));
		if (TraversalState.MatchesTagExact(UGT.Traversal_State_FreeRoam))
		{
			TriggerTraversalAction(false);
		}
	}

	//DrawDebugSphere(GetWorld(), SkeletalMesh->GetSocketLocation(FName("root")), 5.f, 8, FColor::Purple);
	//DrawDebugSphere(GetWorld(), SkeletalMesh->GetSocketLocation(FName("ik_hand_l")), 5.f, 8, FColor::Purple);
	//DrawDebugSphere(GetWorld(), SkeletalMesh->GetSocketLocation(FName("ik_hand_r")), 5.f, 8, FColor::Purple);
	//ClimbMovementIK();
}

void UTraversalComponent::TriggerTraversalAction(bool JumpAction)
{
	if (TraversalAction.MatchesTagExact(FUmbraGameplayTags::Get().Traversal_Action_NoAction))
	{
		WallHitResult = DetectWall();
		if (!WallHitResult.bBlockingHit && JumpAction)
		{
			OwnerCharacter->Jump();
			return;
		}

		GridScan(4, 30, WallHitResult.ImpactPoint, ReverseNormal(WallHitResult.ImpactNormal));
		MeasureWall();
		DecideTraversalType(JumpAction);
	}
}

void UTraversalComponent::GridScan(int GridWidth, int GridHeight, const FVector& ScanBaseLocation,
	const FRotator& ScanRotation)
{
	if (!FindWallEdge(GridWidth, GridHeight, ScanBaseLocation, ScanRotation, WallEdgeResult))
	{
		return;
	}

	// Если состояние не "Climb", вычисляем поворот стены на основе обратного нормали
	WallRotation = ScanRotation;
	if (!TraversalState.MatchesTagExact(FUmbraGameplayTags::Get().Traversal_State_Climb))
	{
		WallRotation = ReverseNormal(WallEdgeResult.ImpactNormal);
	}
	
	FHitResult LastTopHit;
	bool bEndOfWallFound = false;
	if (!FindWallTop(WallEdgeResult, WallRotation, WallTopResult, LastTopHit, bEndOfWallFound))
	{
		return;
	}

	if (!bEndOfWallFound) return;
	
	// Если состояние не FreeRoam, дальнейшая обработка не требуется
	if (!TraversalState.MatchesTagExact(FUmbraGameplayTags::Get().Traversal_State_FreeRoam))
	{
		return;
	}
	
	if (!FindWallDepth(LastTopHit, WallRotation, WallDepthResult))
	{
		return;
	}
	
	if (!FindWallVault(WallDepthResult, WallRotation, WallVaultResult))
	{
		return;
	}
}

void UTraversalComponent::PlayTraversalMontage()
{
	SetTraversalState(CurrentActionData.InState);

	FVector TopResultWarpLocation = FindWarpLocation(WallTopResult.ImpactPoint, WallRotation, CurrentActionData.Warp1XOffset, CurrentActionData.Warp1ZOffset);
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("TopResultWarp", TopResultWarpLocation, WallRotation);

	FVector BalanceWarpLocation = FindWarpLocation(WallTopResult.ImpactPoint, WallRotation, CurrentActionData.Warp2XOffset, CurrentActionData.Warp2ZOffset);
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("BalanceWarp", BalanceWarpLocation, WallRotation);

	if (AnimInstance)
	{

		FOnMontageBlendingOutStarted BlendingOutStarted = FOnMontageBlendingOutStarted::CreateWeakLambda(this,
			[this](UAnimMontage*, bool)
		{
				SetTraversalState(CurrentActionData.OutState);
		}
		);
		
		AnimInstance->Montage_Play(CurrentActionData.Montage);
		AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutStarted);
		SetTraversalAction(UGT.Traversal_Action_NoAction);
	}
}

bool UTraversalComponent::FindWallEdge(int GridWidth, int GridHeight, const FVector& ScanBaseLocation,
                                       const FRotator& ScanRotation, FHitResult& OutWallEdgeResult)
{
	 TArray<FHitResult> WallHitTraces;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);

    for (int32 i = 0; i < GridWidth; i++)
    {
        float ColumnOffset = i * GridColumnSpacing - GridWidth * GridColumnOffsetMultiplier;
        FVector ScanColumnOrigin  = VectorDirectionMoveWithRotation(ScanBaseLocation, FUmbraGameplayTags::Get().Traversal_Direction_Right, ColumnOffset, ScanRotation);
        
        TArray<FHitResult> LineHitTraces;
        for (int32 j = 0; j < GridHeight; j++)
        {
            FVector GridPointLocation = VectorDirectionMove(ScanColumnOrigin, FUmbraGameplayTags::Get().Traversal_Direction_Up, j * GridRowSpacing);
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

bool UTraversalComponent::FindWallTop(const FHitResult& WallEdgeHit, const FRotator& WallRot,
	FHitResult& OutWallTopResult, FHitResult& OutLastTopHit, bool& OutEndOfWallFound)
{
	bool bFoundTop = false;
	FHitResult LastTopHit;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
	
	for (int32 i = 0; i < TopScanIterations; i++)
	{
		FVector ForwardScanLocation = VectorDirectionMoveWithRotation(WallEdgeHit.ImpactPoint, FUmbraGameplayTags::Get().Traversal_Direction_Forward, i * TopForwardScanStep, WallRot);
		FVector ElevatedTraceStart = VectorDirectionMove(ForwardScanLocation, FUmbraGameplayTags::Get().Traversal_Direction_Up, TopElevatedOffset);
		FVector ElevatedTraceEnd = VectorDirectionMove(ElevatedTraceStart, FUmbraGameplayTags::Get().Traversal_Direction_Down, TopElevatedOffset);
		FHitResult TopHit;

		GetWorld()->SweepSingleByChannel(
			TopHit,
			ElevatedTraceStart,
			ElevatedTraceEnd,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(TopSweepSphereRadius),
			QueryParams);

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
				OutEndOfWallFound = true;
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

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
	
	FHitResult DepthResult;
	GetWorld()->SweepSingleByChannel(
		DepthResult,
		VectorDirectionMoveWithRotation(LastTopHit.ImpactPoint, UGT.Traversal_Direction_Forward, DepthForwardOffset, WallRot),
		LastTopHit.ImpactPoint,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(DepthSweepSphereRadius),
		QueryParams);

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

bool UTraversalComponent::FindWallVault(const FHitResult& WallDepthHit, const FRotator& WallRot,
	FHitResult& OutWallVaultResult)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
	
	FHitResult VaultHit;
	FVector Start = VectorDirectionMoveWithRotation(WallDepthHit.ImpactPoint, FUmbraGameplayTags::Get().Traversal_Direction_Forward, VaultForwardOffset, WallRot);
	FVector End = VectorDirectionMove(Start, FUmbraGameplayTags::Get().Traversal_Direction_Down, VaultDownOffset);

	GetWorld()->SweepSingleByChannel(
		VaultHit,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(VaultSweepSphereRadius),
		QueryParams);
	
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
	
	if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UTraversalInterface::StaticClass()))
	{
		ITraversalInterface::Execute_SetTraversalState(AnimInstance, NewTraversalState);
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

	if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UTraversalInterface::StaticClass()))
	{
		ITraversalInterface::Execute_SetClimbStyle(AnimInstance, NewClimbStyle);
	}
}

void UTraversalComponent::SetClimbDirection(const FGameplayTag& NewClimbDirection)
{
	if (NewClimbDirection.MatchesTagExact(ClimbDirection))
	{
		return;
	}

	ClimbDirection = NewClimbDirection;

	if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UTraversalInterface::StaticClass()))
	{
		ITraversalInterface::Execute_SetClimbDirection(AnimInstance, NewClimbDirection);
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

void UTraversalComponent::MeasureWall()
{
	if (!WallHitResult.bBlockingHit || !WallTopResult.bBlockingHit)
	{
		WallHeight = 0.f;
		WallDepth = 0.f;
		VaultHeight = 0.f;
		return;
	}

	WallHeight = WallTopResult.ImpactPoint.Z - SkeletalMesh->GetSocketLocation("root").Z;
	WallDepth = WallDepthResult.bBlockingHit ? FVector::Dist(WallTopResult.ImpactPoint, WallDepthResult.ImpactPoint) : 0.f;
	VaultHeight = WallDepthResult.bBlockingHit && WallVaultResult.bBlockingHit ? WallDepthResult.ImpactPoint.Z - WallVaultResult.ImpactPoint.Z : 0.f;

	UE_LOG(LogTemp, Warning, TEXT("Wall Height: [%f], Wall Depth: [%f], Vault Height: [%f]"), WallHeight, WallDepth, VaultHeight);
}

void UTraversalComponent::DecideTraversalType(bool JumpAction)
{
	if (!WallEdgeResult.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("DecideTraversalType: [No Action]"))
		SetTraversalAction(UGT.Traversal_Action_NoAction);
		if (JumpAction) OwnerCharacter->Jump();
	}

	if (TraversalState.MatchesTagExact(UGT.Traversal_State_Climb))
	{
		UE_LOG(LogTemp, Warning, TEXT("DecideTraversalType: [Climb or Hope]"));
	}

	if (TraversalState.MatchesTagExact(UGT.Traversal_State_FreeRoam))
	{
		if (bInLand)
		{
			if (WallHeight >= 90.f && WallHeight <= 160.f)
			{
				if (WallDepth >= 0.f && WallDepth <= 120.f)
				{
					if (WallDepth >= 60.f)
					{
						if (CharacterMovement->Velocity.Length() > 20)
						{
							UE_LOG(LogTemp, Warning, TEXT("DecideTraversalType: [Vault]"))
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("DecideTraversalType: [Mantle]"))
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("DecideTraversalType: [Mantle]"))
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("DecideTraversalType: [Mantle]"))
				}
			}
			else
			{
				if (WallHeight < 250.f)
				{
					UE_LOG(LogTemp, Warning, TEXT("DecideTraversalType: [Climb]"))
					DecideClimbStyle(WallTopResult.ImpactPoint, WallRotation);
					NextClimbResult = WallTopResult;
					ClimbStyle.MatchesTagExact(UGT.Traversal_ClimbStyle_BracedClimb) ? SetTraversalAction(UGT.Traversal_Action_BracedClimb) : SetTraversalAction(UGT.Traversal_Action_FreeHang);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("DecideTraversalType: [No Action]"))
					SetTraversalAction(UGT.Traversal_Action_NoAction);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("DecideTraversalType: [Climb]"))
		}
	}
}

void UTraversalComponent::ResetTraversalResults()
{
	WallHitResult = FHitResult();
	WallEdgeResult = FHitResult();
	WallTopResult = FHitResult();
	WallDepthResult = FHitResult();
	WallVaultResult = FHitResult();

	WallHeight = 0;
	WallDepth = 0;
	VaultHeight = 0;
	WallRotation = FRotator::ZeroRotator;
}

void UTraversalComponent::SetTraversalAction(const FGameplayTag& NewTraversalAction)
{
	if (NewTraversalAction.MatchesTagExact(TraversalAction))
	{
		return;
	}

	TraversalAction = NewTraversalAction;
	if (ITraversalInterface *TraversalAnimInstance = Cast<ITraversalInterface>(AnimInstance))
	{
		TraversalAnimInstance->SetTraversalAction(NewTraversalAction);
	}

	if (TraversalAction.MatchesTagExact(UGT.Traversal_Action_NoAction))
	{
		ResetTraversalResults();
	}
	else
	{
		CurrentActionData = TraversalActionDataMap.FindRef(NewTraversalAction);
		PlayTraversalMontage();
	}
}

FVector UTraversalComponent::FindWarpLocation(const FVector& Location, const FRotator& Rotation, float XOffset,
	float ZOffset)
{
	FVector FirstStepVector = VectorDirectionMoveWithRotation(Location, UGT.Traversal_Direction_Forward, XOffset, Rotation);
	return VectorDirectionMove(FirstStepVector, UGT.Traversal_Direction_Up, ZOffset);
}

void UTraversalComponent::DecideClimbStyle(const FVector& Location, const FRotator& Rotation)
{
	FVector FirstStepVector = VectorDirectionMoveWithRotation(Location, UGT.Traversal_Direction_Forward, 125.f, Rotation);
	FVector TraceStart = VectorDirectionMoveWithRotation(FirstStepVector, UGT.Traversal_Direction_Backward, 10.f, Rotation);
	FVector TraceEnd = VectorDirectionMoveWithRotation(FirstStepVector, UGT.Traversal_Direction_Forward, 25.f, Rotation);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
	
	FHitResult HitResult;
	GetWorld()->SweepSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(10.f),
		QueryParams);
	
	HitResult.bBlockingHit ? SetClimbStyle(FUmbraGameplayTags::Get().Traversal_ClimbStyle_BracedClimb) : SetClimbStyle(FUmbraGameplayTags::Get().Traversal_ClimbStyle_FreeHang);
}

void UTraversalComponent::NextClimbHandIK(const bool bLeftHand)
{
	if (!TraversalState.MatchesTagExact(UGT.Traversal_State_ReadyToClimb))
	{
		return;
	}

	if (!NextClimbResult.bBlockingHit)
	{
		return;
	}

	for (int32 i = 0; i < 5; ++i)
	{
		float MoveValue = bLeftHand ? (8 - i * 2) * -1.f : (8 - i * 2) * 1.f;
		FVector TempVector = VectorDirectionMoveWithRotation(NextClimbResult.ImpactPoint, UGT.Traversal_Direction_Right, MoveValue, WallRotation);
		FVector TraceStartVector = VectorDirectionMoveWithRotation(TempVector, UGT.Traversal_Direction_Backward, 20.f, WallRotation);
		FVector TraceEndVector = VectorDirectionMoveWithRotation(TempVector, UGT.Traversal_Direction_Forward, 20.f, WallRotation);

		FHitResult ClimbWallHitResult;
		bool bHit = UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			TraceStartVector,
			TraceEndVector,
			5.f,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForDuration,
			ClimbWallHitResult,
			true,
			FLinearColor::Black,
			FLinearColor::Black,
			5.f);

		if (!ClimbWallHitResult.bBlockingHit)
		{
			continue;
		}
		
		WallRotation = ReverseNormal(ClimbWallHitResult.ImpactNormal);
		FRotator ClimbHandRotation = bLeftHand ? WallRotation + FRotator(90.f, 0, 280.f) : WallRotation + FRotator(270.f, 0, 270.f);
		
		for (int32 j = 0; j < 6; j++)
		{
			TempVector = VectorDirectionMoveWithRotation(ClimbWallHitResult.ImpactPoint, UGT.Traversal_Direction_Forward, 2.f, WallRotation);
			TraceStartVector = VectorDirectionMove(TempVector, UGT.Traversal_Direction_Up, j * 5);
			TraceEndVector = VectorDirectionMove(TempVector, UGT.Traversal_Direction_Down, 75.f);

			FHitResult ClimbTopHitResult;
			bHit = UKismetSystemLibrary::SphereTraceSingle(
				GetWorld(),
				TraceStartVector,
				TraceEndVector,
				2.5f,
				UEngineTypes::ConvertToTraceType(ECC_Visibility),
				false,
				TArray<AActor*>(),
				EDrawDebugTrace::None,
				ClimbTopHitResult,
				true,
				FLinearColor::Blue,
				FLinearColor::Blue,
				5.f);

			if (!ClimbTopHitResult.bBlockingHit || ClimbTopHitResult.bStartPenetrating)
			{
				continue;
			}

			FVector ClimbHandLocation = FVector(
				ClimbWallHitResult.ImpactPoint.X,
				ClimbWallHitResult.ImpactPoint.Y,
				VectorDirectionMove(ClimbTopHitResult.ImpactPoint, UGT.Traversal_Direction_Down, 9.f).Z);
			
			if (bLeftHand)
			{
				if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UTraversalInterface::StaticClass()))
				{
					ITraversalInterface::Execute_SetLeftHandClimbLocation(AnimInstance, ClimbHandLocation);
					ITraversalInterface::Execute_SetLeftHandClimbRotation(AnimInstance, ClimbHandRotation);
				}
			}
			else
			{
				if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UTraversalInterface::StaticClass()))
				{
					ITraversalInterface::Execute_SetRightHandClimbLocation(AnimInstance, ClimbHandLocation);
					ITraversalInterface::Execute_SetRightHandClimbRotation(AnimInstance, ClimbHandRotation);
				}
			}
			
			return;
		}
	}
}

void UTraversalComponent::UpdateHandIK(const bool bLeftHand)
{
	if (!TraversalState.MatchesTagExact(UGT.Traversal_State_Climb))
	{
		return;
	}
	
	FHitResult ClimbWallHitResult;
	for (int32 i = 0; i < 5; i++)
	{
		float MoveValue = ClimbHandSpace + i * 2;
		FVector Shift; 
		if (bLeftHand)
		{
			Shift = VectorDirectionMoveWithRotation(
				SkeletalMesh->GetSocketLocation(FName("ik_hand_l")),
				UGT.Traversal_Direction_Right,
				MoveValue,
				OwnerCharacter->GetActorRotation());
		}
		else
		{
			Shift = VectorDirectionMoveWithRotation(
				SkeletalMesh->GetSocketLocation(FName("ik_hand_r")),
				UGT.Traversal_Direction_Left,
				MoveValue,
				OwnerCharacter->GetActorRotation());
		}
		
		FVector TraceStart = VectorDirectionMoveWithRotation(Shift, UGT.Traversal_Direction_Backward, 50.f, OwnerCharacter->GetActorRotation());
		FVector TraceEnd = VectorDirectionMoveWithRotation(Shift, UGT.Traversal_Direction_Forward, 70.f, OwnerCharacter->GetActorRotation());
		
		UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			TraceStart,
			TraceEnd,
			15.f,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForOneFrame,
			ClimbWallHitResult,
			true);

		if (ClimbWallHitResult.bBlockingHit && !ClimbWallHitResult.bStartPenetrating) break;
	}

	if (!ClimbWallHitResult.bBlockingHit || ClimbWallHitResult.bStartPenetrating)
	{
		UE_LOG(LogTemp, Error, TEXT("Wall | BlockingHit = [%d] | StartPenetrating = [%d]"), ClimbWallHitResult.bBlockingHit, ClimbWallHitResult.bStartPenetrating);
		return;
	}

	WallRotation = ReverseNormal(ClimbWallHitResult.ImpactNormal);
	FRotator ClimbHandRotation = bLeftHand ? WallRotation + FRotator(90.f, 0, 280.f) : WallRotation + FRotator(270.f, 0, 270.f);
	
	FHitResult ClimbTopHitResult;
	for (int32 i = 0; i < 6; i++)
	{
		float MoveValue = i * 5;
		FVector Shift = VectorDirectionMoveWithRotation(ClimbWallHitResult.ImpactPoint, UGT.Traversal_Direction_Forward, 2.f, WallRotation);
		FVector TraceStart =  VectorDirectionMove(Shift, UGT.Traversal_Direction_Up, MoveValue);
		FVector TraceEnd = VectorDirectionMove(TraceStart, UGT.Traversal_Direction_Down, 75.f);
		
		UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			TraceStart,
			TraceEnd,
			2.5f,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::None,
			ClimbTopHitResult,
			true);

		if (ClimbTopHitResult.bBlockingHit && !ClimbTopHitResult.bStartPenetrating) break;
	}

	if (!ClimbTopHitResult.bBlockingHit || ClimbTopHitResult.bStartPenetrating)
	{
		UE_LOG(LogTemp, Error, TEXT("TOP | BlockingHit = [%d] | StartPenetrating = [%d]"), ClimbTopHitResult.bBlockingHit, ClimbTopHitResult.bStartPenetrating);
		return;
	}

	FVector ClimbHandLocation = FVector(
	ClimbWallHitResult.ImpactPoint.X,
	ClimbWallHitResult.ImpactPoint.Y,
	VectorDirectionMove(ClimbTopHitResult.ImpactPoint, UGT.Traversal_Direction_Down, 9.f).Z);

	UE_LOG(LogTemp, Warning, TEXT("LH = %d | Location = [X = %f, Y = %f, Z = %f]"), bLeftHand, ClimbHandLocation.X, ClimbHandLocation.Y, ClimbHandLocation.Z);
	
	if (bLeftHand)
	{
		if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UTraversalInterface::StaticClass()))
		{
			ITraversalInterface::Execute_SetLeftHandClimbLocation(AnimInstance, ClimbHandLocation);
			ITraversalInterface::Execute_SetLeftHandClimbRotation(AnimInstance, ClimbHandRotation);
		}
	}
	else
	{
		if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UTraversalInterface::StaticClass()))
		{
			ITraversalInterface::Execute_SetRightHandClimbLocation(AnimInstance, ClimbHandLocation);
			ITraversalInterface::Execute_SetRightHandClimbRotation(AnimInstance, ClimbHandRotation);
		}
	}
}

void UTraversalComponent::ClimbMovementIK()
{
	if (!TraversalState.MatchesTagExact(UGT.Traversal_State_Climb))
	{
		return;
	}

	UpdateHandIK(false);
	UpdateHandIK(true);
}


void UTraversalComponent::ValidateIsInLand()
{
	if (TraversalState.MatchesTagExact(UGT.Traversal_State_Climb))
	{
		bInLand = false;
		return;
	}

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
	
	bInLand = GetWorld()->SweepSingleByChannel(
		HitResult,
		SkeletalMesh->GetSocketLocation("root"),
		SkeletalMesh->GetSocketLocation("root"),
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeBox(FVector(10.f, 10.f, 4.f)),
		QueryParams);
}

bool UTraversalComponent::ValidateClimbMovementSurface(const FVector& MovementImpactLocation)
{
	FRotator CharacterRotation = OwnerCharacter->GetActorRotation();
	FVector FirstTempVector = VectorDirectionMoveWithRotation(MovementImpactLocation, UGT.Traversal_Direction_Right, RightValue * 13.f, CharacterRotation);
	FVector SecondTempVector = VectorDirectionMove(FirstTempVector, UGT.Traversal_Direction_Down, 90.f);
	FVector TraceStart = VectorDirectionMoveWithRotation(SecondTempVector, UGT.Traversal_Direction_Backward, 40.f, CharacterRotation);
	FVector TraceEnd = VectorDirectionMoveWithRotation(SecondTempVector, UGT.Traversal_Direction_Backward, 25.f, CharacterRotation);

	FHitResult HitResult;
	UKismetSystemLibrary::CapsuleTraceSingle(
		this,
		TraceStart,
		TraceEnd,
		5.f,
		82.f,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true);

	return !HitResult.bBlockingHit;
}

void UTraversalComponent::AddMovementInput(float ScaleValue, bool Front)
{
	Front ? ForwardValue = ScaleValue : RightValue = ScaleValue;

	if (TraversalState.MatchesTagExact(UGT.Traversal_State_FreeRoam))
	{
		const FRotator YawRotation(0.f, OwnerCharacter->GetControlRotation().Yaw, 0.f);
		FVector WorldLocation = Front ? FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) : FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		OwnerCharacter->AddMovementInput(WorldLocation, ScaleValue, false);
	}

	else if (TraversalState.MatchesTagExact(UGT.Traversal_State_Climb))
	{
		AnimInstance->IsAnyMontagePlaying() ? StopClimbMovement() : ClimbMovement();
	}
}

void UTraversalComponent::ResetMovement()
{
	ForwardValue = 0.f;
	RightValue = 0.f;
	SetClimbDirection(UGT.Traversal_Direction_NoDirection);
}

void UTraversalComponent::ClimbMovement()
{
	if (TraversalState.MatchesTagExact(UGT.Traversal_Action_CornerMove))
	{
		return;
	}

	if (FMath::Abs(RightValue) < 0.7f)
	{
		StopClimbMovement();
		return;
	}

	SetClimbDirection(RightValue > 0.f ? UGT.Traversal_Direction_Right : UGT.Traversal_Direction_Left);

	FHitResult ClimbWallHitResult;
	FHitResult ClimbTopHitResult;
	int32 Index = 0;
	for (; Index <= 2; Index++)
	{
		FVector TempVector = VectorDirectionMoveWithRotation(
		SkeletalMesh->GetComponentLocation() + FVector(0.f, 0.f, 195.f),
			UGT.Traversal_Direction_Right,
			ClimbMoveCheckDistance * RightValue,
			OwnerCharacter->GetActorRotation());

		FVector TraceStart = VectorDirectionMove(TempVector, UGT.Traversal_Direction_Down, Index * 10.f);
		FVector TraceEnd = VectorDirectionMoveWithRotation(TraceStart, UGT.Traversal_Direction_Forward, 60.f, OwnerCharacter->GetActorRotation());

		//UE_LOG(LogTemp, Display, TEXT("ClimbMovement: RightValue: [%f], Index: [%d]"), RightValue, Index);
		//UE_LOG(LogTemp, Display, TEXT("ClimbMovement: TraceStart: [%s], TraceEnd: [%s]"), *TraceStart.ToString(), *TraceEnd.ToString());
		
		UKismetSystemLibrary::SphereTraceSingle(
			this,
			TraceStart,
			TraceEnd,
			5.f,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForOneFrame,
			ClimbWallHitResult,
			true
		);


		if (!ClimbWallHitResult.bStartPenetrating)
		{
			if (!ClimbWallHitResult.bBlockingHit)
			{
				if (Index != 2) StopClimbMovement();
			}
			else
			{
				WallRotation = ReverseNormal(ClimbWallHitResult.ImpactNormal);
		
				for (int32 i = 0; i <= 6; i++)
				{
		
					FVector FirstTempVector = VectorDirectionMoveWithRotation(ClimbWallHitResult.ImpactPoint, UGT.Traversal_Direction_Forward, 2.f, WallRotation);
					FVector SecondTempVector = VectorDirectionMove(FirstTempVector, UGT.Traversal_Direction_Up,  5.f);
					FVector InnerTraceStart = VectorDirectionMove(SecondTempVector, UGT.Traversal_Direction_Up, i * 5.f);
					FVector InnerTraceEnd = VectorDirectionMove(InnerTraceStart, UGT.Traversal_Direction_Down, 100.f);
		
					UKismetSystemLibrary::SphereTraceSingle(
						this,
						InnerTraceStart,
						InnerTraceEnd,
						2.5f,
						UEngineTypes::ConvertToTraceType(ECC_Visibility),
						false,
						TArray<AActor*>(),
						EDrawDebugTrace::ForOneFrame,
						ClimbTopHitResult,
						true);
		
					if (ClimbTopHitResult.bStartPenetrating && Index == 2 && i == 6) StopClimbMovement();
					else break;
				}
				
				if (!ClimbTopHitResult.bBlockingHit) StopClimbMovement();
				else break;
			}
		}
	}
	
	if (ClimbCheckForSides(ClimbTopHitResult.ImpactPoint)) return;
	if (!ValidateClimbMovementSurface(ClimbWallHitResult.ImpactPoint))
	{
		StopClimbMovement();
		return;
	}
	
	FVector HorizontalLocation = VectorDirectionMoveWithRotation(ClimbWallHitResult.ImpactPoint, UGT.Traversal_Direction_Backward, ClimbValues(ClimbStyle, 44.f, 7.f), WallRotation);
	float NewLocationZ = ClimbTopHitResult.ImpactPoint.Z;
	
	SetNewClimbPosition(HorizontalLocation.X, HorizontalLocation.Y, NewLocationZ, WallRotation);
	DecideClimbStyle(ClimbTopHitResult.ImpactPoint, WallRotation);
}

void UTraversalComponent::StopClimbMovement()
{
	CharacterMovement->StopMovementImmediately();
	SetClimbDirection(UGT.Traversal_Direction_NoDirection);
}

bool UTraversalComponent::ClimbCheckForSides(const FVector& ImpactPoint)
{
	for (int32 i = 0; i <= 5; i++)
	{

		FVector TempVector = VectorDirectionMove(ImpactPoint, UGT.Traversal_Direction_Up, 2.f);
		FVector TraceStart = VectorDirectionMove(TempVector, UGT.Traversal_Direction_Up, i * 5.f);
		FVector TraceEnd = VectorDirectionMoveWithRotation(TraceStart, UGT.Traversal_Direction_Right, RightValue * 15.f, WallRotation);

		FHitResult HitResult;
		UKismetSystemLibrary::LineTraceSingle(
			this,
			TraceStart,
			TraceEnd,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForOneFrame,
			HitResult,
			true);

		if (HitResult.bBlockingHit && i == 5)
		{
			StopClimbMovement();
			return true;
		}
		if (!HitResult.bBlockingHit)
		{
			break;
		}
	}
	return false;
}

void UTraversalComponent::SetNewClimbPosition(float NewLocationX, float NewLocationY, float NewLocationZ,
	FRotator NewRotation)
{
	float X = FMath::FInterpTo(OwnerCharacter->GetActorLocation().X, NewLocationX, GetWorld()->GetDeltaSeconds(), 2.f);
	float Y = FMath::FInterpTo(OwnerCharacter->GetActorLocation().Y, NewLocationY, GetWorld()->GetDeltaSeconds(), 2.f);
	float Z = FMath::FInterpTo(OwnerCharacter->GetActorLocation().Z, NewLocationZ - ClimbValues(ClimbStyle, 107.f, 115.f), GetWorld()->GetDeltaSeconds(), 0.f);
	FVector NewActorLocation = FVector(X, Y, Z);	
	
	OwnerCharacter->SetActorLocationAndRotation(NewActorLocation, NewRotation);
}






