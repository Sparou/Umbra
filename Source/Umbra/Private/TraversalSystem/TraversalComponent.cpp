// Copyrighted by Vorona Games


#include "TraversalSystem/TraversalComponent.h"
#include "MotionWarpingComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TraversalSystem/TraversalInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(TraversalComponent)

UTraversalComponent::UTraversalComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTraversalComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeReferences();
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
		if (TraversalState.MatchesTagExact(UGT.Traversal_State_FreeRoam))
		{
			TriggerTraversalAction(false);
		}
	}
	
	ClimbMovementIK();
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

/********************* Setters **********************/

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

void UTraversalComponent::SetNewClimbPosition(float NewLocationX, float NewLocationY, float NewLocationZ,
	FRotator NewRotation)
{
	float X = FMath::FInterpTo(
		OwnerCharacter->GetActorLocation().X,
		NewLocationX, GetWorld()->GetDeltaSeconds(),
		2.f);
	
	float Y = FMath::FInterpTo(
		OwnerCharacter->GetActorLocation().Y,
		NewLocationY, GetWorld()->GetDeltaSeconds(),
		2.f);

	float Z = FMath::FInterpTo(
		OwnerCharacter->GetActorLocation().Z,
		NewLocationZ - ClimbValues(ClimbStyle, ClimbMovementBracedZOffset, ClimbMovementFreeHangZOffset),
		GetWorld()->GetDeltaSeconds(), 0.f);
	
	FVector NewActorLocation = FVector(X, Y, Z);	
	
	OwnerCharacter->SetActorLocationAndRotation(NewActorLocation, NewRotation);
}

/********************* Wall Detection **********************/

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
				UE_LOG(TraversalComponent, Log, TEXT("Wall Impact Point: [%s]"), *HitResult.ImpactPoint.ToString());
				return HitResult;
			}
		}
	}

	return HitResult;
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

bool UTraversalComponent::FindWallEdge(int GridWidth, int GridHeight, const FVector& ScanBaseLocation,
                                       const FRotator& ScanRotation, FHitResult& OutWallEdgeResult)
{

	/* Алгоритм поиска края стены с помощью сетки трейсов:
	 * 1. Находится нижняя точка столбца
	 * 2. От нижней точки столбца вверх пускаются трейсы
	 * 3. Все трейсы заносятся в массив
	 * 4. Все трейсы проверяются на резкий скачок расстояния попадания (если такой имеется, значит мы нашли край стены)
	 * 5. Из всех найденных краев выбирается тот, что ближе всего к персонажу
	 */
	
	TArray<FHitResult> EdgesResults;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
 
    for (int32 i = 0; i <= GridWidth; i++)
    {
        float ColumnOffset = i * GridColumnSpacing - GridWidth * GridColumnOffsetMultiplier;
        FVector ScanColumnOrigin  = VectorDirectionMoveWithRotation(ScanBaseLocation, FUmbraGameplayTags::Get().Traversal_Direction_Right, ColumnOffset, ScanRotation);
        
        TArray<FHitResult> LineHitTraces;
        for (int32 j = 0; j <= GridHeight; j++)
        {
            FVector GridPointLocation = VectorDirectionMove(ScanColumnOrigin, FUmbraGameplayTags::Get().Traversal_Direction_Up, j * GridRowSpacing);
            FHitResult LineHit;
        	
        	GetWorld()->LineTraceSingleByChannel(
        		LineHit,
        		VectorDirectionMoveWithRotation(GridPointLocation, FUmbraGameplayTags::Get().Traversal_Direction_Backward, GridScanLineTraceDistance, ScanRotation),
        		VectorDirectionMoveWithRotation(GridPointLocation, FUmbraGameplayTags::Get().Traversal_Direction_Forward, GridScanLineTraceDistance, ScanRotation),
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
                EdgesResults.Add(PreviousHitResult);
                break;
            }
        }
    }

    if (EdgesResults.Num() <= 0)
    {
        return false;
    }
	
    OutWallEdgeResult = EdgesResults[0];
    FVector OwnerLocation = OwnerCharacter->GetActorLocation();
    for (int32 i = 1; i < EdgesResults.Num(); i++)
    {
        float NewDistance = FVector::DistSquared(OwnerLocation, EdgesResults[i].ImpactPoint);
        float CurrentDistance = FVector::DistSquared(OwnerLocation, OutWallEdgeResult.ImpactPoint);
        if (NewDistance <= CurrentDistance)
        {
            OutWallEdgeResult = EdgesResults[i];
        }
    }

	if (bDrawWallEdgeDebug)
	{
		DrawDebugSphere(GetWorld(), OutWallEdgeResult.ImpactPoint, 4.f, 8, FColor::Blue, false, 3.f);
	}
	UE_LOG(TraversalComponent, Log, TEXT("Wall Edge Location: [%s]"), *WallEdgeResult.ImpactPoint.ToString());
	
    return OutWallEdgeResult.bBlockingHit && !OutWallEdgeResult.bStartPenetrating;
}

bool UTraversalComponent::FindWallTop(const FHitResult& WallEdgeHit, const FRotator& WallRot,
	FHitResult& OutWallTopResult, FHitResult& OutLastTopHit, bool& OutEndOfWallFound)
{

	/* Алгоритм поиска верха стены с помощью сетки трейсов:
	 * 1. От наденного раннее края стены пускаем поднятые трейсы снизу вверх
	 * 2. Если мы попали на первой итерации мы нашли вверх стены
	 * 3. Следующие итерации ищут точки верхнй поверхности стены
	 * 4. Если в какой-то момент трейс не сопрекоснулся с поверхность, значит был найден конец стены
	 */
	
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
				DrawDebugSphere(GetWorld(), OutWallTopResult.ImpactPoint, 2.f, 4, FColor::Orange, false, 3.f);
			}
			bFoundTop = true;
			UE_LOG(TraversalComponent, Log, TEXT("Wall Top Location: [%s]"), *OutWallTopResult.ImpactPoint.ToString());
		}
		
		if (i != 0)
		{
			if (TopHit.bBlockingHit)
			{
				LastTopHit = TopHit;
				if (bDrawWallTopDebug)
				{
					DrawDebugSphere(GetWorld(), LastTopHit.ImpactPoint, 6.f, 12, FColor::Green, false, 3.f);
				}
				UE_LOG(TraversalComponent, Log, TEXT("Wall Surface Found: [%s]"), *LastTopHit.ImpactPoint.ToString());
			}
			else
			{
				OutEndOfWallFound = true;
				UE_LOG(TraversalComponent, Log, TEXT("End Of Wall Found"))
				break;
			}
		}
	}
	
	OutLastTopHit = LastTopHit;

	if (!bFoundTop)
	{
		UE_LOG(TraversalComponent, Warning, TEXT("Wall Top Not Found!"));
	}
	
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
		UE_LOG(TraversalComponent, Warning, TEXT("Wall Depth Not Found!"));
		return false;
	}

	OutWallDepthResult = DepthResult;
	
	if (bDrawWallDepthDebug)
	{
		DrawDebugSphere(GetWorld(), OutWallDepthResult.ImpactPoint, 4.f, 8, FColor::Red, false, 3.f);
	}
	UE_LOG(TraversalComponent, Log, TEXT("Wall Depth Location: [%s]"), *OutWallDepthResult.ImpactPoint.ToString());
	
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
		UE_LOG(TraversalComponent, Warning, TEXT("Wall Vault Not Found!"));
		return false;
	}

	OutWallVaultResult = VaultHit;
	if (bDrawWallVaultDebug)
	{
		DrawDebugSphere(GetWorld(), OutWallVaultResult.ImpactPoint, 10.f, 8, FColor::White, false, 6.f);
	}
	UE_LOG(TraversalComponent, Log, TEXT("Wall Vault Location: [%s]"), *OutWallVaultResult.ImpactPoint.ToString());

	
	return true;
}

FVector UTraversalComponent::FindWarpLocation(const FVector& Location, const FRotator& Rotation, float XOffset,
	float ZOffset)
{
	FVector XOffsetVector = VectorDirectionMoveWithRotation(Location, UGT.Traversal_Direction_Forward, XOffset, Rotation);
	FVector ZOffsetVector = VectorDirectionMove(XOffsetVector, UGT.Traversal_Direction_Up, ZOffset);
	DrawDebugSphere(GetWorld(), ZOffsetVector, 8.f, 16, FColor::Red, false, 3.f);
	UE_LOG(TraversalComponent, Log, TEXT("Warp Location: [%s]"), *ZOffsetVector.ToString());
	return ZOffsetVector;
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
	WallDepth = WallTopResult.bBlockingHit && WallDepthResult.bBlockingHit ? FVector::Dist(WallTopResult.ImpactPoint, WallDepthResult.ImpactPoint) : 0.f;
	VaultHeight = WallDepthResult.bBlockingHit && WallVaultResult.bBlockingHit ? WallDepthResult.ImpactPoint.Z - WallVaultResult.ImpactPoint.Z : 0.f;

	UE_LOG(TraversalComponent, Log, TEXT("Wall Height: [%f], Wall Depth: [%f], Vault Height: [%f]"), WallHeight, WallDepth, VaultHeight);
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

float UTraversalComponent::ClimbValues(const FGameplayTag& NewClimbStyle, const float& Braced, const float& FreeHang) const
{
	return ClimbStyle.MatchesTagExact(FUmbraGameplayTags::Get().Traversal_ClimbStyle_BracedClimb) ? Braced : FreeHang;
}

void UTraversalComponent::DecideTraversalType(bool JumpAction)
{
	if (!WallTopResult.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("DecideTraversalType: [No Action]"))
		SetTraversalAction(UGT.Traversal_Action_NoAction);
		if (JumpAction) OwnerCharacter->Jump();
	}

	if (TraversalState.MatchesTagExact(UGT.Traversal_State_Climb))
	{
		UE_LOG(TraversalComponent, Log, TEXT("TraversalType: [Climb or Hope]"));
	}

	if (TraversalState.MatchesTagExact(UGT.Traversal_State_FreeRoam))
	{
		if (bInLand)
		{
			if (WallHeight >= 45.f && WallHeight <= 160.f)
			{
				if (WallDepth >= 0.f && WallDepth <= 120.f)
				{
					if (VaultHeight >= 60.f && VaultHeight <= 120.f)
					{
						if (CharacterMovement->Velocity.Length() > 20)
						{
							UE_LOG(TraversalComponent, Log, TEXT("DecideTraversalType: [%s]"), *UGT.Traversal_State_Vault.ToString());
						}
						else
						{
							UE_LOG(TraversalComponent, Log, TEXT("DecideTraversalType: [%s]"), *UGT.Traversal_State_Mantle.ToString());
						}
					}
					else
					{
						UE_LOG(TraversalComponent, Warning, TEXT("DecideTraversalType: [%s]"), *UGT.Traversal_State_Mantle.ToString());
					}
				}
				else
				{
					UE_LOG(TraversalComponent, Warning, TEXT("DecideTraversalType: [%s]"), *UGT.Traversal_State_Mantle.ToString());
				}
			}
			else
			{
				if (WallHeight < 250.f)
				{
					UE_LOG(TraversalComponent, Log, TEXT("DecideTraversalType: [%s]"), *UGT.Traversal_State_Climb.ToString());
					DecideClimbStyle(WallTopResult.ImpactPoint, WallRotation);
					NextClimbResult = WallTopResult;
					ClimbStyle.MatchesTagExact(UGT.Traversal_ClimbStyle_BracedClimb) ? SetTraversalAction(UGT.Traversal_Action_BracedClimb) : SetTraversalAction(UGT.Traversal_Action_FreeHang);
				}
				else
				{
					UE_LOG(TraversalComponent, Log, TEXT("DecideTraversalType: [%s]"), *UGT.Traversal_Action_NoAction.ToString());
					SetTraversalAction(UGT.Traversal_Action_NoAction);
				}
			}
		}
		else
		{
			UE_LOG(TraversalComponent, Log, TEXT("DecideTraversalType: [%s]"), *UGT.Traversal_State_Climb.ToString());
		}
	}
	else
	{
		UE_LOG(TraversalComponent, Log, TEXT("DecideTraversalType: [%s]"), *UGT.Traversal_State_Climb.ToString());
		DecideClimbStyle(WallTopResult.ImpactPoint, WallRotation);
		NextClimbResult = WallTopResult;
		ClimbStyle.MatchesTagExact(UGT.Traversal_ClimbStyle_BracedClimb) ? SetTraversalAction(UGT.Traversal_Action_BracedClimb) : SetTraversalAction(UGT.Traversal_Action_FreeHang);
	}
}

void UTraversalComponent::DecideClimbStyle(const FVector& Location, const FRotator& Rotation)
{
	FVector FirstStepVector = VectorDirectionMove(Location, UGT.Traversal_Direction_Down, DecideClimbStyleDownOffset);
	FVector TraceStart = VectorDirectionMoveWithRotation(FirstStepVector, UGT.Traversal_Direction_Backward, DecideClimbStyleBackwardOffset, Rotation);
	FVector TraceEnd = VectorDirectionMoveWithRotation(FirstStepVector, UGT.Traversal_Direction_Forward, DecideClimbStyleForwardOffset, Rotation);
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
	
	FHitResult HitResult;

	UKismetSystemLibrary::SphereTraceSingle(
		this,
		TraceStart,
		TraceEnd,
		DecideClimbStyleSphereRadius,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		TArray<AActor*>(),
		bDecideClimbStyleEnableDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult,
		true
	);
		
	if (HitResult.bBlockingHit)
	{
		SetClimbStyle(FUmbraGameplayTags::Get().Traversal_ClimbStyle_BracedClimb);
		UE_LOG(TraversalComponent, Log, TEXT("DecideClimbStyle: [%s]"), *UGT.Traversal_ClimbStyle_BracedClimb.ToString());
	}
	else
	{
		SetClimbStyle(FUmbraGameplayTags::Get().Traversal_ClimbStyle_FreeHang);
		UE_LOG(TraversalComponent, Log, TEXT("DecideClimbStyle: [%s]"), *UGT.Traversal_ClimbStyle_FreeHang.ToString());
	}
}


/********************* Climbing **********************/

void UTraversalComponent::AddMovementInput(float ScaleValue, bool Front)
{
	Front ? ForwardValue = ScaleValue : RightValue = ScaleValue;

	if (TraversalState.MatchesTagExact(UGT.Traversal_State_FreeRoam))
	{
		const FRotator YawRotation(0.f, OwnerCharacter->GetControlRotation().Yaw, 0.f);
		FVector WorldDirection = Front ? FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) : FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		OwnerCharacter->AddMovementInput(WorldDirection, ScaleValue, false);
	}

	else if (TraversalState.MatchesTagExact(UGT.Traversal_State_Climb))
	{
		AnimInstance->IsAnyMontagePlaying() ? StopClimbMovement() : ClimbMovement();
	}
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

	float ZOffset = 0.f;
	if (ClimbStyle.MatchesTagExact(UGT.Traversal_ClimbStyle_BracedClimb))
	{
		ZOffset = ClimbMovementWallDetectionBracedHeightOffset;
	}
	else
	{
		ZOffset = ClimbMovementWallDetectionFreeHangHeightOffset;
	}
	
	FVector WallDetectionInitialVector = VectorDirectionMoveWithRotation(
	SkeletalMesh->GetComponentLocation() + FVector(0.f, 0.f, ZOffset),
		UGT.Traversal_Direction_Right,
		ClimbMovementWallDetectionHorizontalOffset * RightValue,
		OwnerCharacter->GetActorRotation());
	
	int32 WallDetectionIndex = 0;
	for (; WallDetectionIndex < ClimbMovementWallDetectionIterations; WallDetectionIndex++)
	{

		/*
		 * Внешний цикл проверяет наличие стены, по
		 * которой можно карабкаться с помощью серии
		 * вертикальный трейсов.
		 */
		
		FVector WallDetectionTraceStart = VectorDirectionMove(
			WallDetectionInitialVector,
			UGT.Traversal_Direction_Down,
			WallDetectionIndex * ClimbMovementWallDetectionVerticalMultiplier);
		
		FVector WallDetectionTraceEnd = VectorDirectionMoveWithRotation(
			WallDetectionTraceStart,
			UGT.Traversal_Direction_Forward,
			ClimbMovementWallDetectionDistance, OwnerCharacter->GetActorRotation());
		
		UKismetSystemLibrary::SphereTraceSingle(
			this,
			WallDetectionTraceStart,
			WallDetectionTraceEnd,
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
				if (WallDetectionIndex != ClimbMovementWallDetectionIterations - 1)
				{
					StopClimbMovement();
				}
			}
			else
			{
				WallRotation = ReverseNormal(ClimbWallHitResult.ImpactNormal);
				
				for (int32 TopDetectionIndex = 0; TopDetectionIndex < ClimbMovementTopDetectionIterations; TopDetectionIndex++)
				{
					/*
					 * Внутренний цикл ищет верх стены с помощью серии
					 * вертикальных трейсов, направленных от чуть смещенной
					 * вперед и наверх ClimbWallHit вниз.
					 */
		
					FVector TopDetectionForwardOffsetVector =
						VectorDirectionMoveWithRotation(
							ClimbWallHitResult.ImpactPoint,
							UGT.Traversal_Direction_Forward,
							ClimbMovementTopDetectionInitialForwardOffset,
							WallRotation);
					
					FVector TopDetectionInitialVector = VectorDirectionMove(
						TopDetectionForwardOffsetVector,
						UGT.Traversal_Direction_Up,
						ClimbMovementTopDetectionInitialVerticalOffset);
					
					FVector TopDetectionTraceStart = VectorDirectionMove(
						TopDetectionInitialVector,
						UGT.Traversal_Direction_Up,
						TopDetectionIndex * ClimbMovementTopDetectionVerticalOffsetMultiplier);
					
					FVector TopDetectionTraceEnd = VectorDirectionMove(
						TopDetectionTraceStart, UGT.Traversal_Direction_Down,
						ClimbMovementTopDetectionDistance);
		
					UKismetSystemLibrary::SphereTraceSingle(
						this,
						TopDetectionTraceStart,
						TopDetectionTraceEnd,
						2.5f,
						UEngineTypes::ConvertToTraceType(ECC_Visibility),
						false,
						TArray<AActor*>(),
						EDrawDebugTrace::ForOneFrame,
						ClimbTopHitResult,
						true);
		
					if (ClimbTopHitResult.bStartPenetrating
						&& WallDetectionIndex == (ClimbMovementWallDetectionIterations - 1)
						&& TopDetectionIndex == (ClimbMovementTopDetectionIterations - 1))
					{
						StopClimbMovement();
					}
					else break;
				}
				
				if (!ClimbTopHitResult.bBlockingHit)
				{
					StopClimbMovement();
				}
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
	
	FVector HorizontalLocation = VectorDirectionMoveWithRotation(
		ClimbWallHitResult.ImpactPoint,
		UGT.Traversal_Direction_Backward,
		ClimbValues(ClimbStyle, ClimbMovementBracedXOffset, ClimbMovementFreeHangXOffset),
		WallRotation);
	
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
	for (int32 i = 0; i < ClimbCheckForSidesIterations; i++)
	{

		FVector InitialVector = VectorDirectionMove(
			ImpactPoint,
			UGT.Traversal_Direction_Up,
			ClimbCheckForSidesInitialVerticalOffset);
		
		FVector TraceStart = VectorDirectionMove(
			InitialVector,
			UGT.Traversal_Direction_Up,
			i * ClimbCheckForSidesVerticalOffsetMultiplier);
		
		FVector TraceEnd = VectorDirectionMoveWithRotation(
			TraceStart,
			UGT.Traversal_Direction_Right,
			RightValue * ClimbCheckForSidesHorizontalOffset, WallRotation);

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

		if (HitResult.bBlockingHit && i == ClimbCheckForSidesIterations - 1)
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

bool UTraversalComponent::ValidateClimbMovementSurface(const FVector& MovementImpactLocation)
{
	FRotator CharacterRotation = OwnerCharacter->GetActorRotation();
	
	FVector HorizontalOffsetVector = VectorDirectionMoveWithRotation(
		MovementImpactLocation,
		UGT.Traversal_Direction_Right,
		RightValue * ClimbMovementSurfaceValidationHorizontalOffset,
		CharacterRotation);
	
	FVector InitialVector = VectorDirectionMove(
		HorizontalOffsetVector,
		UGT.Traversal_Direction_Down,
		ClimbMovementSurfaceValidationInitialVerticalOffset);
	
	FVector TraceStart = VectorDirectionMoveWithRotation(
		InitialVector,
		UGT.Traversal_Direction_Backward,
		ClimbMovementSurfaceValidationCapsuleStartOffset,
		CharacterRotation);
	
	FVector TraceEnd = VectorDirectionMoveWithRotation(
		InitialVector,
		UGT.Traversal_Direction_Backward,
		ClimbMovementSurfaceValidationCapsuleEndOffset,
		CharacterRotation);
	
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
	
	FHitResult ClimbWallHitResult;
	FHitResult ClimbTopHitResult;
	FRotator ClimbHandRotation;

	int32 WallDetectionMaxIndex = HandIKMaxDistance / 2;
	
	for (int32 WallDetectionIndex = 0; WallDetectionIndex < WallDetectionMaxIndex; ++WallDetectionIndex)
	{
		float MoveValue = bLeftHand ? (WallDetectionMaxIndex - WallDetectionIndex * 2) * -1.f : (WallDetectionMaxIndex - WallDetectionIndex * 2) * 1.f;
		FVector WallDetectionInitialVector = VectorDirectionMoveWithRotation(NextClimbResult.ImpactPoint, UGT.Traversal_Direction_Right, MoveValue, WallRotation);
		FVector TraceStart = VectorDirectionMoveWithRotation(WallDetectionInitialVector, UGT.Traversal_Direction_Backward, HandIKTraceDistance, WallRotation);
		FVector TraceEnd = VectorDirectionMoveWithRotation(WallDetectionInitialVector, UGT.Traversal_Direction_Forward, HandIKTraceDistance, WallRotation);
		
		bool bHit = UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			TraceStart,
			TraceEnd,
			HandIKWallDetectionSphereRadius,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForDuration,
			ClimbWallHitResult,
			true);

		if (!ClimbWallHitResult.bBlockingHit)
		{
			continue;
		}
		
		WallRotation = ReverseNormal(ClimbWallHitResult.ImpactNormal);
		ClimbHandRotation = bLeftHand ? WallRotation + HandIKLeftHandAdditiveRotation : WallRotation + HandIKRightHandAdditiveRotation;
		
		for (int32 TopDetectionIndex = 0; TopDetectionIndex < HandIKTopDetectionIterations; TopDetectionIndex++)
		{
			FVector TopDetectionInitialVector = VectorDirectionMoveWithRotation(ClimbWallHitResult.ImpactPoint, UGT.Traversal_Direction_Forward, 2.f, WallRotation);
			TraceStart = VectorDirectionMove(TopDetectionInitialVector, UGT.Traversal_Direction_Up, TopDetectionIndex * HandIKTopDetectionVerticalOffsetMultiplier);
			TraceEnd = VectorDirectionMove(TopDetectionInitialVector, UGT.Traversal_Direction_Down, HandIKTopDetectionTraceDistance);
			
			UKismetSystemLibrary::SphereTraceSingle(
				GetWorld(),
				TraceStart,
				TraceEnd,
				HandIKTopDetectionSphereRadius,
				UEngineTypes::ConvertToTraceType(ECC_Visibility),
				false,
				TArray<AActor*>(),
				EDrawDebugTrace::None,
				ClimbTopHitResult,
				true);

			if (!ClimbTopHitResult.bBlockingHit || ClimbTopHitResult.bStartPenetrating)
			{
				continue;
			}
			
			break;
		}

		break;
	}

	FVector ClimbHandLocation = FVector(
		ClimbWallHitResult.ImpactPoint.X,
		ClimbWallHitResult.ImpactPoint.Y,
		VectorDirectionMove(ClimbTopHitResult.ImpactPoint, UGT.Traversal_Direction_Down, HandIKLocationVerticalOffset).Z);

	UE_LOG(TraversalComponent, Log, TEXT("Next Climb IK: Location = [%s]"), *ClimbHandLocation.ToString());
	UE_LOG(TraversalComponent, Log, TEXT("Next Climb IK: Rotation = [%s]"), *ClimbHandRotation.ToString());
	
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

void UTraversalComponent::NextClimbFootIK(const bool bLeftFoot)
{
	if (!TraversalState.MatchesTagExact(UGT.Traversal_State_ReadyToClimb))
	{
		return;
	}

	FHitResult ClimbWallHitResult;
	
	for (int32 WallDetectionIndex = 0; WallDetectionIndex < FootIKWallDetectionIterations; WallDetectionIndex++)
	{
		FVector InitialVerticalOffset = VectorDirectionMove(
			NextClimbResult.ImpactPoint,
			UGT.Traversal_Direction_Up,
			WallDetectionIndex * FootIKWallDetectionVerticalOffsetMultiplier);

		FVector InitialHorizontalOffset;
		
		if (bLeftFoot)
		{
			InitialHorizontalOffset = VectorDirectionMoveWithRotation(
				InitialVerticalOffset,
				UGT.Traversal_Direction_Left,
				FootIKLeftHorizontalOffset,
				WallRotation);
		}
		else
		{
			InitialHorizontalOffset = VectorDirectionMoveWithRotation(
				InitialVerticalOffset,
				UGT.Traversal_Direction_Right,
				FootIKRightHorizontalOffset,
				WallRotation);
		}

		FVector SecondaryHorizontalOffset = VectorDirectionMove(
			InitialHorizontalOffset,
			UGT.Traversal_Direction_Down,
			bLeftFoot ? FootIKLeftVerticalOffset : FootIKRightVerticalOffset);

		FVector TraceStart = VectorDirectionMoveWithRotation(
			SecondaryHorizontalOffset,
			UGT.Traversal_Direction_Backward,
			FootIKTraceBackwardOffset,
			WallRotation);

		FVector TraceEnd = VectorDirectionMoveWithRotation(
			SecondaryHorizontalOffset,
			UGT.Traversal_Direction_Forward,
			FootIKTraceForwardOffset,
			WallRotation);

		UKismetSystemLibrary::SphereTraceSingle(
			this,
			TraceStart,
			TraceEnd,
			FootIKWallDetectionSphereRadius,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForDuration,
			ClimbWallHitResult,
			true);

		if (ClimbWallHitResult.bBlockingHit)
		{
			break;
		}
	}
	
	FVector FootIKLocation = VectorDirectionMoveWithRotation(
		ClimbWallHitResult.ImpactPoint,
		UGT.Traversal_Direction_Backward,
		FootIKLocationAdditiveBackwardOffset,
		ReverseNormal(ClimbWallHitResult.ImpactNormal));
	
	UE_LOG(TraversalComponent, Log, TEXT("Next Foot IK: Foot Location = [%s]"), *ClimbWallHitResult.ToString());
	
	if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UTraversalInterface::StaticClass()))
	{
		if (bLeftFoot)
		{
			ITraversalInterface::Execute_SetLeftFootClimbLocation(AnimInstance, FootIKLocation);
		}
		else
		{
			ITraversalInterface::Execute_SetRightFootClimbLocation(AnimInstance, FootIKLocation);
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
	FHitResult ClimbTopHitResult;
	
	for (int32 WallDetectionIndex = 0; WallDetectionIndex < HandIKUpdateWallDetectionIterations; WallDetectionIndex++)
	{
		float MoveValue = HandIKUpdateClimbHandSpace + WallDetectionIndex * HandIKUpdateWallDetectionHorizontalOffsetMultiplier;
		FVector InitialVector;
		
		if (bLeftHand)
		{
			InitialVector = VectorDirectionMoveWithRotation(
				SkeletalMesh->GetSocketLocation(FName("ik_hand_l")),
				UGT.Traversal_Direction_Right,
				MoveValue,
				OwnerCharacter->GetActorRotation());
		}
		else
		{
			InitialVector = VectorDirectionMoveWithRotation(
				SkeletalMesh->GetSocketLocation(FName("ik_hand_r")),
				UGT.Traversal_Direction_Left,
				MoveValue,
				OwnerCharacter->GetActorRotation());
		}
		
		FVector TraceStart = VectorDirectionMoveWithRotation(
			InitialVector,
			UGT.Traversal_Direction_Backward,
			HandIKUpdateWallDetectionTraceStartOffset,
			OwnerCharacter->GetActorRotation());
		
		FVector TraceEnd = VectorDirectionMoveWithRotation(
			InitialVector,
			UGT.Traversal_Direction_Forward,
			HandIKUpdateWallDetectionTraceDistance,
			OwnerCharacter->GetActorRotation());
		
		UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			TraceStart,
			TraceEnd,
			HandIKUpdateWallDetectionSphereRadius,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForOneFrame,
			ClimbWallHitResult,
			true);

		if (ClimbWallHitResult.bBlockingHit && !ClimbWallHitResult.bStartPenetrating)
		{
			break;
		}
	}

	if (!ClimbWallHitResult.bBlockingHit || ClimbWallHitResult.bStartPenetrating)
	{
		return;
	}

	WallRotation = ReverseNormal(ClimbWallHitResult.ImpactNormal);
	FRotator ClimbHandRotation = bLeftHand ? WallRotation + HandIKLeftHandAdditiveRotation : WallRotation + HandIKRightHandAdditiveRotation;
	
	for (int32 TopDetectionIndex = 0; TopDetectionIndex < HandIKUpdateTopDetectionIterations; TopDetectionIndex++)
	{
		FVector InitialVector = VectorDirectionMoveWithRotation(
			ClimbWallHitResult.ImpactPoint,
			UGT.Traversal_Direction_Forward,
			HandIKUpdateTopDetectionForwardOffset,
			WallRotation);
		
		FVector TraceStart = VectorDirectionMove(
			InitialVector,
			UGT.Traversal_Direction_Up,
			TopDetectionIndex * HandIKUpdateTopDetectionVerticalOffsetMultiplier);
		
		FVector TraceEnd = VectorDirectionMove(
			TraceStart,
			UGT.Traversal_Direction_Down,
			HandIKUpdateTopDetectionTraceDistance);
		
		UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			TraceStart,
			TraceEnd,
			HandIKUpdateTopDetectionSphereRadius,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::None,
			ClimbTopHitResult,
			true);

		if (ClimbTopHitResult.bBlockingHit && !ClimbTopHitResult.bStartPenetrating)
		{
			break;
		}
	}

	if (!ClimbTopHitResult.bBlockingHit || ClimbTopHitResult.bStartPenetrating)
	{
		return;
	}

	FVector ClimbHandLocation = FVector(
		ClimbWallHitResult.ImpactPoint.X,
		ClimbWallHitResult.ImpactPoint.Y,
		VectorDirectionMove(ClimbTopHitResult.ImpactPoint, UGT.Traversal_Direction_Down, 9.f).Z);

	//UE_LOG(TraversalComponent, Log, TEXT("Update Climb IK: Hand = [%d] | Location = [%s]"), bLeftHand, *ClimbHandLocation.ToString());
	//UE_LOG(TraversalComponent, Log, TEXT("Update Climb IK: Hand = [%d] | Rotation = [%s]"), bLeftHand, *ClimbHandRotation.ToString());
	
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

void UTraversalComponent::UpdateFootIK(const bool bLeftFoot)
{
	if (!TraversalState.MatchesTagExact(UGT.Traversal_State_Climb))
	{
		return;
	}

	if (!AnimInstance->GetCurveValue(bLeftFoot ? "LeftFootIK" : "RightFootIK") == 1.f)
	{
		ResetFootIK();
		return;
	}

	FHitResult ClimbWallHitResult;

	for (int32 WallDetectionIndex = 0; WallDetectionIndex < FootIKUpdateWallDetectionIterations; WallDetectionIndex++)
	{
		FVector HandLocation = FVector(
			bLeftFoot ? SkeletalMesh->GetSocketLocation("ik_foot_l").X : SkeletalMesh->GetSocketLocation("ik_foot_r").X,
			bLeftFoot ? SkeletalMesh->GetSocketLocation("ik_foot_l").Y : SkeletalMesh->GetSocketLocation("ik_foot_r").Y,
			bLeftFoot ? SkeletalMesh->GetSocketLocation("hand_l").Z : SkeletalMesh->GetSocketLocation("hand_r").Z);

		FVector FirstVerticalOffset = VectorDirectionMove(
			HandLocation,
			UGT.Traversal_Direction_Down,
			bLeftFoot ? FootIKUpdateLeftVerticalOffset : FootIKUpdateRightVerticalOffset);

		FVector SecondVerticalOffset = VectorDirectionMove(
			FirstVerticalOffset,
			UGT.Traversal_Direction_Up,
			WallDetectionIndex * FootIKUpdateVerticalOffsetMultiplier);

		FVector HorizontalOffset;

		if (bLeftFoot)
		{
			HorizontalOffset = VectorDirectionMove(
				SecondVerticalOffset,
				UGT.Traversal_Direction_Right,
				FootIKUpdateLeftHorizontalOffset);
		}
		else
		{
			HorizontalOffset = VectorDirectionMove(
				SecondVerticalOffset,
				UGT.Traversal_Direction_Left,
				FootIKUpdateRightHorizontalOffset);
		}

		FVector TraceStart = VectorDirectionMove(
			HorizontalOffset,
			UGT.Traversal_Direction_Backward,
			FootIKUpdateTraceBackwardOffset);

		FVector TraceEnd = VectorDirectionMove(
			HorizontalOffset,
			UGT.Traversal_Direction_Forward,
			FootIKUpdateTraceForwardOffset);

		UKismetSystemLibrary::SphereTraceSingle(
			this,
			TraceStart,
			TraceEnd,
			FootIKUpdateWallDetectionSphereRadius,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForOneFrame,
			ClimbWallHitResult,
			true);

		if (ClimbWallHitResult.bBlockingHit && !ClimbWallHitResult.bStartPenetrating)
		{
			break;
		}
	}

	if (!ClimbWallHitResult.bBlockingHit || ClimbWallHitResult.bStartPenetrating)
	{
		return;
	}
	
	FVector FootIKLocation = VectorDirectionMoveWithRotation(
		ClimbWallHitResult.ImpactPoint,
		UGT.Traversal_Direction_Backward,
		FootIKLocationAdditiveBackwardOffset,
		ReverseNormal(ClimbWallHitResult.ImpactNormal));

	if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UTraversalInterface::StaticClass()))
	{
		if (bLeftFoot)
		{
			ITraversalInterface::Execute_SetLeftFootClimbLocation(AnimInstance, FootIKLocation);
		}
		else
		{
			ITraversalInterface::Execute_SetRightFootClimbLocation(AnimInstance, FootIKLocation);
		}
	}
}

void UTraversalComponent::ResetFootIK()
{
	if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UTraversalInterface::StaticClass()))
	{
		ITraversalInterface::Execute_SetRightFootClimbLocation(AnimInstance, SkeletalMesh->GetSocketLocation("ik_foot_r"));
		ITraversalInterface::Execute_SetLeftFootClimbLocation(AnimInstance, SkeletalMesh->GetSocketLocation("ik_foot_l"));
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
	UpdateFootIK(false);
	UpdateFootIK(true);
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




void UTraversalComponent::ResetMovement()
{
	ForwardValue = 0.f;
	RightValue = 0.f;
	SetClimbDirection(UGT.Traversal_Direction_NoDirection);
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
}




