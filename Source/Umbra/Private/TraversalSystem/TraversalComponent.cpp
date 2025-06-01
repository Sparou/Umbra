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
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(TraversalComponentLog)

UTraversalComponent::UTraversalComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UTraversalComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeReferences();
}

void UTraversalComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UTraversalComponent, TraversalState);
	DOREPLIFETIME(UTraversalComponent, TraversalAction);
	DOREPLIFETIME(UTraversalComponent, ClimbStyle);
	DOREPLIFETIME(UTraversalComponent, ClimbDirection);
	DOREPLIFETIME(UTraversalComponent, bIsDropping);
}

void UTraversalComponent::InitializeReferences()
{
	OwnerCharacter = Cast<ACharacter>(this->GetOwner());
	checkf(OwnerCharacter, TEXT("Traversal Component: Owner Character is null"));
	
	CharacterMovement = OwnerCharacter->GetCharacterMovement();
	checkf(CharacterMovement, TEXT("Traversal Component: Character Movement is null"));

	MotionWarping = OwnerCharacter->FindComponentByClass<UMotionWarpingComponent>();
	checkf(MotionWarping, TEXT("Traversal Component: Motion Warping is null"));

	//Camera = OwnerCharacter->FindComponentByClass<UCameraComponent>();
	//checkf(Camera, TEXT("Traversal Component: Camera is null"));

	Capsule = OwnerCharacter->FindComponentByClass<UCapsuleComponent>();
	checkf(Capsule, TEXT("Traversal Component: Capsule is null"));

	SkeletalMesh = OwnerCharacter->GetMesh();
	checkf(SkeletalMesh, TEXT("Traversal Component: Skeletal Mesh is NUll"));

	AnimInstance = SkeletalMesh->GetAnimInstance();
	//checkf(AnimInstance, TEXT("Traversal Component: Anim Instance is null"));

}

void UTraversalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// if (GetOwnerRole() == ROLE_Authority && GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Green, *WallTopResult.ImpactPoint.ToString());
	// 	GEngine->AddOnScreenDebugMessage(2, 0, FColor::Blue, *TraversalState.ToString());
	// 	GEngine->AddOnScreenDebugMessage(3, 0, FColor::Red, *TraversalAction.ToString());
	// 	GEngine->AddOnScreenDebugMessage(4, 0, FColor::Yellow, *ClimbStyle.ToString());
	// 	GEngine->AddOnScreenDebugMessage(5, 0, FColor::Black, *ClimbDirection.ToString());
	// }
	
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
		if (TraversalState.MatchesTagExact(UGT.Traversal_State_FreeRoam) && !bIsDropping)
		{
			if (GetOwnerRole() < ROLE_Authority)
				ServerTriggerTraversalAction(false);
			else
				TriggerTraversalAction(false);
		}
	}
	if (GetNetMode() != NM_DedicatedServer)
	{
		ClimbMovementIK();
	}
}

void UTraversalComponent::TriggerTraversalAction(bool JumpAction)
{
	if (TraversalAction.MatchesTagExact(UGT.Traversal_Action_NoAction))
	{
		WallHitResult = DetectWall();
		if (!WallHitResult.bBlockingHit && JumpAction)
		{
			//OwnerCharacter->Jump();
			MulticastJump();
			return;
		}  

		float GridScanHeight;
		
		if(GetNetMode() == NM_DedicatedServer)
		{
			GridScanHeight = TraversalStateValues(
				TraversalState,
				WallHitResult.ImpactPoint.Z,
				0.f,
				ClimbStyle.MatchesTagExact(UGT.Traversal_ClimbStyle_BracedClimb) ?
						OwnerCharacter->GetActorLocation().Z + ClimbMovementDedicatedServerBracedHandZOffset :
						OwnerCharacter->GetActorLocation().Z + ClimbMovementDedicatedServerFreeHangHandZOffset,
				0.f,
				0.f);
		}
		else
		{
			GridScanHeight = TraversalStateValues(
				TraversalState,
				WallHitResult.ImpactPoint.Z,
				0.f,
				GetCharacterHandHeight(),
				0.f,
				0.f);
		}
		
		FVector GridScanLocation = FVector(
			WallHitResult.ImpactPoint.X,
			WallHitResult.ImpactPoint.Y,
			GridScanHeight);
		
		float GridHeight = TraversalStateValues(TraversalState, 30.f, 0.f, 7.f, 0.f, 0.f);
		float GridWidth = TraversalStateValues(TraversalState, 4.f, 0.f, 2.f, 0.f, 30.f);

		// UE_LOG(TraversalComponentLog, Log, TEXT("Grid Scan Height: [%f]"), GridScanHeight);
		// UE_LOG(TraversalComponentLog, Log, TEXT("Grid Scan Width: [%f]"), GridWidth);
		// UE_LOG(TraversalComponentLog, Log, TEXT("Grid Scan Location: [%s]"), *GridScanLocation.ToString());
		
		GridScan(GridWidth, GridHeight, GridScanLocation, ReverseNormal(WallHitResult.ImpactNormal));
		MeasureWall();
		DecideTraversalType(JumpAction);
	}
}

void UTraversalComponent::PlayTraversalMontage()
{
	if (WallTopResult.ImpactPoint == FVector::ZeroVector)
	{
		//UE_LOG(TraversalComponentLog, Warning, TEXT("Play Traversal Montage: Wall Top Result is zero!"));
		SetTraversalAction(UGT.Traversal_Action_NoAction);
		return;
	}

	SetTraversalState(CurrentActionData.InState);
	
	FVector TopResultWarpLocation = FindWarpLocation(WallTopResult.ImpactPoint, WallRotation, CurrentActionData.Warp1XOffset, CurrentActionData.Warp1ZOffset);
	//MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("TopResultWarp", TopResultWarpLocation, WallRotation);

	FVector BalanceWarpLocation = FindWarpLocation(WallTopResult.ImpactPoint, WallRotation, CurrentActionData.Warp2XOffset, CurrentActionData.Warp2ZOffset);
	//MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("BalanceWarp", BalanceWarpLocation, WallRotation);

	FVector DepthResultWarpLocation = FindWarpLocation(WallDepthResult.ImpactPoint, WallRotation, CurrentActionData.Warp2XOffset, CurrentActionData.Warp2ZOffset);
	//MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("DepthResultWarp", DepthResultWarpLocation, WallRotation);

	FVector VaultResultWarpLocation = FindWarpLocation(WallVaultResult.ImpactPoint, WallRotation, CurrentActionData.Warp3XOffset, CurrentActionData.Warp3ZOffset);
	//MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("VaultResultWarp", VaultResultWarpLocation, WallRotation);

	if(GetOwnerRole() == ROLE_Authority)
	{
		MulticastPlayTraversalMontage(
			TraversalAction,
			TopResultWarpLocation,
			BalanceWarpLocation,
			DepthResultWarpLocation,
			VaultResultWarpLocation,
			WallRotation,
			NextClimbResult
		);
	}
	
}

void UTraversalComponent::MulticastPlayTraversalMontage_Implementation(
	FGameplayTag ActionTag,
	FVector TopResultWarpLocation,
	FVector BalanceWarpLocation,
	FVector DepthResultWarpLocation,
	FVector VaultResultWarpLocation,
	FRotator WallRot,
	FHitResult NextClimb)
{
	
	CurrentActionData = TraversalActions->FindActionDataByTag(ActionTag);
	WallRotation = WallRot;
	NextClimbResult = NextClimb;
	
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("TopResultWarp", TopResultWarpLocation, WallRot);
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("BalanceWarp",BalanceWarpLocation, WallRot);
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("DepthResultWarp", DepthResultWarpLocation, WallRot);
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("VaultResultWarp", VaultResultWarpLocation, WallRot);
	
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CurrentActionData.Montage);

		if (GetOwnerRole() == ROLE_Authority)
		{
			FOnMontageEnded Ended = FOnMontageEnded::CreateWeakLambda(this, [this](UAnimMontage*, bool)
			{
					SetTraversalAction(UGT.Traversal_Action_NoAction);
			});
			AnimInstance->Montage_SetEndDelegate(Ended);
		}

		FOnMontageBlendingOutStarted BlendingOutStarted = FOnMontageBlendingOutStarted::CreateWeakLambda(this,[this](UAnimMontage*, bool)
		{
			SetTraversalState(CurrentActionData.OutState);
		});
		AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutStarted);
		
	}
}

/********************* Setters **********************/

void UTraversalComponent::SetTraversalState(const FGameplayTag& NewTraversalState)
{
	// if (NewTraversalState.MatchesTagExact(TraversalState))
	// {
	// 	return;
	// }

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
	// if (NewTraversalAction.MatchesTagExact(TraversalAction))
	// {
	// 	return;
	// }

	TraversalAction = NewTraversalAction;

	//UE_LOG(TraversalComponentLog, Log, TEXT("New Traversal Action = [%s]"), *TraversalAction.ToString());
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
		CurrentActionData = TraversalActions->FindActionDataByTag(NewTraversalAction);
		PlayTraversalMontage();
	}
}


void UTraversalComponent::SetNewClimbPosition(float NewLocationX, float NewLocationY, float NewLocationZ,
                                              FRotator NewRotation)
{
	
	FVector CurrentLocation = OwnerCharacter->GetActorLocation();
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	if (DeltaTime < KINDA_SMALL_NUMBER)
		return;

	// Целевая позиция с учётом Z-офсета
	float ZOffset = ClimbValues(ClimbStyle, ClimbMovementBracedZOffset, ClimbMovementFreeHangZOffset);
	FVector DesiredLocation(NewLocationX, NewLocationY, NewLocationZ - ZOffset);

	// Интерполяция вектора константной скоростью ClimbSpeed
	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, DesiredLocation, DeltaTime, ClimbSpeed);

	// Вычисляем нужную скорость, чтобы за один тик попасть в NewLocation
	FVector NeededVelocity = (NewLocation - CurrentLocation) / DeltaTime;
	CharacterMovement->Velocity = NeededVelocity;
	
	OwnerCharacter->SetActorRotation(NewRotation);
}

void UTraversalComponent::OnRep_TraversalState()
{
	SetTraversalState(TraversalState);
}

void UTraversalComponent::OnRep_TraversalAction()
{
	if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UTraversalInterface::StaticClass()))
	{
		ITraversalInterface::Execute_SetTraversalAction(AnimInstance ,TraversalAction);
	}
}

void UTraversalComponent::OnRep_ClimbStyle()
{ 
	if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UTraversalInterface::StaticClass()))
	{
		ITraversalInterface::Execute_SetClimbStyle(AnimInstance ,ClimbStyle);
	}
}

void UTraversalComponent::OnRep_ClimbDirection()
{
	if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UTraversalInterface::StaticClass()))
	{
		ITraversalInterface::Execute_SetClimbDirection(AnimInstance, ClimbDirection);
	}
}

/********************* Wall Detection **********************/

FHitResult UTraversalComponent::DetectWall() const
{
	FHitResult HitResult;
	int32 iterations = CharacterMovement->IsFalling() ? DetectWallIterationsFalling : DetectWallIterationsNotFalling;

	for (int32 i = 0; i < iterations; ++i)
	{
		FVector CurrentLocation = OwnerCharacter->GetActorLocation();

		FVector DownOffset = VectorDirectionMove(CurrentLocation, UGT.Traversal_Direction_Down, DetectWallDownOffset);
		FVector UpOffset = VectorDirectionMove(DownOffset, UGT.Traversal_Direction_Up, i * DetectWallUpStepMultiplier);
		FVector BackwardOffset = VectorDirectionMove(UpOffset, UGT.Traversal_Direction_Backward, DetectWallBackwardOffset);
		FVector ForwardOffset = VectorDirectionMove(UpOffset, UGT.Traversal_Direction_Forward, DetectWallForwardOffset);

		UKismetSystemLibrary::SphereTraceSingle(
			this,
			BackwardOffset,
			ForwardOffset,
			DetectWallSphereRadius,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			GetDebugType(DetectWallDebugType),
			HitResult,
			true,
			DetectWallDebugTraceColor,
			DetectWallDebugHitColor,
			DetectWallDebugTime);
		
		{
			if (HitResult.bBlockingHit && !HitResult.bStartPenetrating)
			{
				//UE_LOG(TraversalComponentLog, Log, TEXT("Wall Impact Point: [%s]"), *HitResult.ImpactPoint.ToString());
				return HitResult;
			}
		}
	}

	return HitResult;
}

void UTraversalComponent::GridScan(int GridWidth, int GridHeight, const FVector& ScanBaseLocation,
	const FRotator& ScanRotation)
{
	if (!FindWallEdge(GridWidth, GridHeight, ScanBaseLocation, ScanRotation, WallEdgeResult)
		|| WallEdgeResult.ImpactPoint == FVector::ZeroVector)
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
	if (!FindWallTop(WallEdgeResult, WallRotation, WallTopResult, LastTopHit, bEndOfWallFound)
		|| WallTopResult.ImpactPoint == FVector::ZeroVector)
	{
		return;
	}

	if (!bEndOfWallFound) return;
	
	// Если состояние не FreeRoam, дальнейшая обработка не требуется
	if (!TraversalState.MatchesTagExact(FUmbraGameplayTags::Get().Traversal_State_FreeRoam))
	{
		return;
	}
	
	if (!FindWallDepth(LastTopHit, WallRotation, WallDepthResult)
		|| WallDepthResult.ImpactPoint == FVector::ZeroVector)
	{
		return;
	}
	
	if (!FindWallVault(WallDepthResult, WallRotation, WallVaultResult)
		|| WallVaultResult.ImpactPoint == FVector::ZeroVector)
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
 
    for (int32 i = 0; i <= GridWidth; i++)
    {
        float ColumnOffset = i * GridColumnSpacing - GridWidth * GridColumnOffsetMultiplier;
        FVector ScanColumnOrigin  = VectorDirectionMoveWithRotation(ScanBaseLocation, FUmbraGameplayTags::Get().Traversal_Direction_Right, ColumnOffset, ScanRotation);
        
        TArray<FHitResult> LineHitTraces;
        for (int32 j = 0; j <= GridHeight; j++)
        {
            FVector GridPointLocation = VectorDirectionMove(
            	ScanColumnOrigin,
            	UGT.Traversal_Direction_Up,
            	j * GridRowSpacing);

        	FVector TraceStart = VectorDirectionMoveWithRotation(
        		GridPointLocation,
        		UGT.Traversal_Direction_Backward,
        		GridScanLineTraceDistance,
        		ScanRotation);

        	FVector TraceEnd = VectorDirectionMoveWithRotation(
        		GridPointLocation,
        		UGT.Traversal_Direction_Forward,
        		GridScanLineTraceDistance,
        		ScanRotation);

        	
            FHitResult LineHit;

        	UKismetSystemLibrary::LineTraceSingle(
        		this,
        		TraceStart,
        		TraceEnd,
        		UEngineTypes::ConvertToTraceType(ECC_Visibility),
        		false,
        		TArray<AActor*>(),
        		GetDebugType(WallEdgeSearchDebugType),
        		LineHit,
        		true,
        		WallEdgeSearchDebugTraceColor,
        		WallEdgeSearchDebugHitColor,
        		WallEdgeSearchDebugTime);
        	
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
	
	//UE_LOG(TraversalComponentLog, Log, TEXT("Wall Edge Location: [%s]"), *WallEdgeResult.ImpactPoint.ToString());
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
	
	for (int32 i = 0; i < WallTopSearchScanIterations; i++)
	{
		FVector ForwardScanLocation = VectorDirectionMoveWithRotation(
			WallEdgeHit.ImpactPoint,
			UGT.Traversal_Direction_Forward,
			i * WallTopSearchForwardScanStep,
			WallRot);
		
		FVector ElevatedTraceStart = VectorDirectionMove(
			ForwardScanLocation,
			UGT.Traversal_Direction_Up,
			WallTopSearchElevatedOffset);
		
		FVector ElevatedTraceEnd = VectorDirectionMove(
			ElevatedTraceStart,
			UGT.Traversal_Direction_Down,
			WallTopSearchElevatedOffset);
		
		FHitResult TopHit;

		UKismetSystemLibrary::SphereTraceSingle(
			this,
			ElevatedTraceStart,
			ElevatedTraceEnd,
			WallTopSearchSphereRadius,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			GetDebugType(WallTopSearchDebugType),
			TopHit,
			true,
			WallTopSearchDebugTraceColor,
			WallTopSearchDebugHitColor,
			WallTopSearchDebugTime);
		
		if (i == 0 && TopHit.bBlockingHit)
		{
			OutWallTopResult = TopHit;
			bFoundTop = true;
			//UE_LOG(TraversalComponentLog, Log, TEXT("Wall Top Location: [%s]"), *OutWallTopResult.ImpactPoint.ToString());
		}
		
		if (i != 0)
		{
			if (TopHit.bBlockingHit)
			{
				LastTopHit = TopHit;
				//UE_LOG(TraversalComponentLog, Log, TEXT("Wall Surface Found: [%s]"), *LastTopHit.ImpactPoint.ToString());
			}
			else
			{
				OutEndOfWallFound = true;
				//(TraversalComponentLog, Log, TEXT("End Of Wall Found"))
				break;
			}
		}
	}
	
	OutLastTopHit = LastTopHit;
	
	if (!bFoundTop)
	{
		//UE_LOG(TraversalComponentLog, Warning, TEXT("Wall Top Not Found!"));
	}
	
	return bFoundTop;
}

bool UTraversalComponent::FindWallDepth(const FHitResult& LastTopHit, const FRotator& WallRot,
	FHitResult& OutWallDepthResult)
{
	FHitResult DepthResult;

	FVector TraceStart = VectorDirectionMoveWithRotation(
		LastTopHit.ImpactPoint,
		UGT.Traversal_Direction_Forward,
		WallDepthSearchForwardOffset,
		WallRot);
	
	UKismetSystemLibrary::SphereTraceSingle(
		this,
		TraceStart,
		LastTopHit.ImpactPoint,
		WallDepthSearchSphereRadius,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		TArray<AActor*>(),
		GetDebugType(WallDepthSearchDebugType),
		DepthResult,
		true,
		WallDepthSearchDebugTraceColor,
		WallDepthSearchDebugHitColor,
		WallDepthSearchDebugTime);

	if (!DepthResult.bBlockingHit)
	{
		//UE_LOG(TraversalComponentLog, Warning, TEXT("Wall Depth Not Found!"));
		return false;
	}

	OutWallDepthResult = DepthResult;
	
	//UE_LOG(TraversalComponentLog, Log, TEXT("Wall Depth Location: [%s]"), *OutWallDepthResult.ImpactPoint.ToString());
	
	return true;
}

bool UTraversalComponent::FindWallVault(const FHitResult& WallDepthHit, const FRotator& WallRot,
	FHitResult& OutWallVaultResult)
{
	FHitResult VaultHit;
	
	FVector TraceStart = VectorDirectionMoveWithRotation(
		WallDepthHit.ImpactPoint,
		UGT.Traversal_Direction_Forward,
		WallVaultSearchForwardOffset,
		WallRot);
	
	FVector TraceEnd = VectorDirectionMove(
		TraceStart,
		UGT.Traversal_Direction_Down,
		WallVaultSearchDownOffset);

	UKismetSystemLibrary::SphereTraceSingle(
		this,
		TraceStart,
		TraceEnd,
		WallVaultSearchSphereRadius,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		TArray<AActor*>(),
		GetDebugType(WallVaultSearchDebugType),
		VaultHit,
		true,
		WallVaultSearchDebugTraceColor,
		WallVaultSearchDebugHitColor,
		WallVaultSearchDebugTime);	

	if (!VaultHit.bBlockingHit)
	{
		//UE_LOG(TraversalComponentLog, Warning, TEXT("Wall Vault Not Found!"));
		return false;
	}

	OutWallVaultResult = VaultHit;
	
	//UE_LOG(TraversalComponentLog, Log, TEXT("Wall Vault Location: [%s]"), *OutWallVaultResult.ImpactPoint.ToString());
	
	return true;
}

FVector UTraversalComponent::FindWarpLocation(const FVector& Location, const FRotator& Rotation, float XOffset,
	float ZOffset) const
{
	FVector XOffsetVector = VectorDirectionMoveWithRotation(Location, UGT.Traversal_Direction_Forward, XOffset, Rotation);
	FVector ZOffsetVector = VectorDirectionMove(XOffsetVector, UGT.Traversal_Direction_Up, ZOffset);
	//UE_LOG(TraversalComponentLog, Log, TEXT("Warp Location: [%s]"), *ZOffsetVector.ToString());
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
	
	
	//UE_LOG(TraversalComponentLog, Log, TEXT("Wall Height: [%f], Wall Depth: [%f], Vault Height: [%f]"), WallHeight, WallDepth, VaultHeight);
}

void UTraversalComponent::DecideTraversalType(bool JumpAction)
{

	//UE_LOG(TraversalComponentLog, Warning, TEXT("DecideTraversalType: WallHeight = [%f]"), WallHeight);
	
	if (!WallTopResult.bBlockingHit)
	{
		//UE_LOG(LogTemp, Warning, TEXT("DecideTraversalType: [No Action]"))
		SetTraversalAction(UGT.Traversal_Action_NoAction);
		if (JumpAction) OwnerCharacter->Jump();
	}

	if (TraversalState.MatchesTagExact(UGT.Traversal_State_Climb))
	{
		//UE_LOG(TraversalComponentLog, Log, TEXT("TraversalType: [Climb or Hope]"));
		DecideClimbOrHope();
		return;
	}

	if (TraversalState.MatchesTagExact(UGT.Traversal_State_FreeRoam))
	{
		if (bInLand)
		{
			//UE_LOG(TraversalComponentLog, Warning, TEXT("DecideTraversalType: bInLand = [%d]"), bInLand);
			if (WallHeight >= MantleMinimumWallHeight && WallHeight <= MantleMaximumWallHeight)
			{
				if (WallDepth >= VaultMinimumDepth && WallDepth <= VaultMaximumDepth)
				{
					if (VaultHeight >= VaultMinimumHeight && VaultHeight <= VaultMaximumHeight)
					{
						if (CharacterMovement->Velocity.Length() > VaultMinimumVelocity)
						{
							if (ValidateVaultSurface())
							{
								//UE_LOG(TraversalComponentLog, Log, TEXT("DecideTraversalType: [%s]"), *UGT.Traversal_State_Vault.ToString());
								SetTraversalAction(UGT.Traversal_Action_Vault);
							}
							else
							{
								SetTraversalAction(UGT.Traversal_Action_NoAction);
							}
						}
						else if (ValidateMantleSurface())
						{
							//UE_LOG(TraversalComponentLog, Log, TEXT("DecideTraversalType: [%s]"), *UGT.Traversal_State_Mantle.ToString());
							SetTraversalAction(UGT.Traversal_Action_Mantle);
						}
					}
					else if (ValidateMantleSurface())
					{
						//UE_LOG(TraversalComponentLog, Warning, TEXT("DecideTraversalType: [%s]"), *UGT.Traversal_State_Mantle.ToString());
						SetTraversalAction(UGT.Traversal_Action_Mantle);
					}
				}
				else if (ValidateMantleSurface())
				{
					//UE_LOG(TraversalComponentLog, Warning, TEXT("DecideTraversalType: [%s]"), *UGT.Traversal_State_Mantle.ToString());
					SetTraversalAction(UGT.Traversal_Action_Mantle);
				}
			}
			else
			{
				if (WallHeight < ClimbMaximumWallHeight && ValidateClimbSurface(WallTopResult.ImpactPoint, WallRotation))
				{
					//UE_LOG(TraversalComponentLog, Log, TEXT("DecideTraversalType: [%s]"), *UGT.Traversal_State_Climb.ToString());
					DecideClimbStyle(WallTopResult.ImpactPoint, WallRotation);
					NextClimbResult = WallTopResult;
					
					ClimbStyle.MatchesTagExact(UGT.Traversal_ClimbStyle_BracedClimb) ?
						SetTraversalAction(UGT.Traversal_Action_BracedClimb) :
						SetTraversalAction(UGT.Traversal_Action_FreeHang);
				}
				else
				{
					//UE_LOG(TraversalComponentLog, Log, TEXT("DecideTraversalType: [%s]"), *UGT.Traversal_Action_NoAction.ToString());
					SetTraversalAction(UGT.Traversal_Action_NoAction);
				}
			}
		}
		else
		{
			//UE_LOG(TraversalComponentLog, Log, TEXT("DecideTraversalType: [%s]"), *UGT.Traversal_State_Climb.ToString());
			DecideClimbStyle(WallTopResult.ImpactPoint, WallRotation);
			NextClimbResult = WallTopResult;
			if (ValidateAirHang() && ValidateClimbSurface(WallTopResult.ImpactPoint, WallRotation))
			{
				ClimbStyle.MatchesTagExact(UGT.Traversal_ClimbStyle_BracedClimb) ?
					SetTraversalAction(UGT.Traversal_Action_BracedClimb_FallingClimb) :
					SetTraversalAction(UGT.Traversal_Action_FreeHang_FallingClimb);
			}
		}
	}
	else if (ValidateClimbSurface(WallTopResult.ImpactPoint, WallRotation))
	{
		//UE_LOG(TraversalComponentLog, Log, TEXT("DecideTraversalType: [%s]"), *UGT.Traversal_State_Climb.ToString());
		DecideClimbStyle(WallTopResult.ImpactPoint, WallRotation);
		NextClimbResult = WallTopResult;
		
		ClimbStyle.MatchesTagExact(UGT.Traversal_ClimbStyle_BracedClimb) ?
			SetTraversalAction(UGT.Traversal_Action_BracedClimb) :
			SetTraversalAction(UGT.Traversal_Action_FreeHang);
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
	}
	else
	{
		SetClimbStyle(FUmbraGameplayTags::Get().Traversal_ClimbStyle_FreeHang);
	}
}

void UTraversalComponent::DecideClimbOrHope()
{
	FGameplayTag ControllerDirection = GetControllerDirection();
	CurrentClimbResult = WallTopResult;
	
	if (ControllerDirection.MatchesTagExact(UGT.Traversal_Direction_Forward) ||
		ControllerDirection.MatchesTagExact(UGT.Traversal_Direction_ForwardRight) ||
		ControllerDirection.MatchesTagExact(UGT.Traversal_Direction_ForwardLeft))
	{
		if (ValidateMantleSurface())
		{
			//UE_LOG(TraversalComponentLog, Log, TEXT("Decide Climb or Hope: ClimbUp"));
			if (ClimbStyle.MatchesTagExact(UGT.Traversal_ClimbStyle_BracedClimb))
			{
				SetTraversalAction(UGT.Traversal_Action_BracedClimb_ClimbUp);
			}
			else
			{
				SetTraversalAction(UGT.Traversal_Action_FreeHang_ClimbUp);
			}
			return;
		}
		else
		{
			//UE_LOG(TraversalComponentLog, Log, TEXT("Decide Climb or Hope: Hope"));
			FindHopLocation();
			DecideClimbStyle(WallTopResult.ImpactPoint, WallRotation);
			NextClimbResult = WallTopResult;
			SetTraversalAction(DecideHopAction());
			return;
		}
	}

	//UE_LOG(TraversalComponentLog, Log, TEXT("Decide Climb or Hope: Hope"));
	FindHopLocation();
	DecideClimbStyle(WallTopResult.ImpactPoint, WallRotation);
	NextClimbResult = WallTopResult;
	SetTraversalAction(DecideHopAction());
}

/********************* Climbing **********************/

void UTraversalComponent::MulticastJump_Implementation()
{
	OwnerCharacter->Jump();
}

void UTraversalComponent::AddMovementInput(float ScaleValue, bool Front)
{
	Front ? ForwardValue = ScaleValue : RightValue = ScaleValue;

	if (TraversalState.MatchesTagExact(UGT.Traversal_State_FreeRoam))
	{
		const FRotator YawRotation(0.f, OwnerCharacter->GetControlRotation().Yaw, 0.f);
		FVector WorldDirection = Front ? FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) : FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		OwnerCharacter->AddMovementInput(WorldDirection, ScaleValue, false);
	}

	else if (TraversalState.MatchesTagExact(UGT.Traversal_State_Climb) && GetOwnerRole())
	{
		//AnimInstance->IsAnyMontagePlaying() ? StopClimbMovement() : ClimbMovement();
		if(GetOwnerRole() == ROLE_Authority)
		{
			AnimInstance->IsAnyMontagePlaying() ? StopClimbMovement() : ClimbMovement();
		}
		else
		{
			AnimInstance->IsAnyMontagePlaying() ? ServerStopClimbMovement() : ServerSendClimbInput(ForwardValue, RightValue);
		}
	}
}

bool UTraversalComponent::ValidateClimbSurface(const FVector& Location, const FRotator& Rotation) const
{
	FVector DownOffset = VectorDirectionMove(
		Location,
		UGT.Traversal_Direction_Down,
		Capsule->GetScaledCapsuleHalfHeight());

	FVector CapsuleTraceLocation = VectorDirectionMoveWithRotation(
		DownOffset,
		UGT.Traversal_Direction_Backward,
		Capsule->GetScaledCapsuleRadius() + ValidateClimbSurfaceAdditiveBackwardOffset,
		Rotation);

	FHitResult ClimbSurface;
	UKismetSystemLibrary::CapsuleTraceSingle(
		this,
		CapsuleTraceLocation,
		CapsuleTraceLocation,
		Capsule->GetScaledCapsuleRadius(),
		Capsule->GetScaledCapsuleHalfHeight(),
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		TArray<AActor*>(),
		GetDebugType(ClimbSurfaceValidationDebugType),
		ClimbSurface,
		true,
		ClimbSurfaceValidationDebugTraceColor,
		ClimbSurfaceValidationDebugHitColor,
		ClimbSurfaceValidationDebugTime);

	return !ClimbSurface.bBlockingHit;
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
			ClimbMovementWallDetectionSphereRadius,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			GetDebugType(ClimbMovementWallDetectionDebugType),
			ClimbWallHitResult,
			true,
			ClimbMovementWallDetectionDebugTraceColor,
			ClimbMovementWallDetectionDebugHitColor,
			ClimbMovementWallDetectionDebugTime);
		
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
						ClimbMovementTopDetectionSphereRadius,
						UEngineTypes::ConvertToTraceType(ECC_Visibility),
						false,
						TArray<AActor*>(),
						GetDebugType(ClimbMovementTopDetectionDebugType),
						ClimbTopHitResult,
						true,
						ClimbMovementTopDetectionDebugTraceColor,
						ClimbMovementTopDetectionDebugHitColor,
						ClimbMovementTopDetectionDebugTime);
		
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

	if (GetOwnerRole() < ROLE_Authority)
	{
		//UE_LOG(TraversalComponentLog, Warning, TEXT("ClimbMovement: [Not Authority]"));
		//UE_LOG(TraversalComponentLog, Log, TEXT("X = [%f], Y = [%f], Z = [%f]"), HorizontalLocation.X, HorizontalLocation.Y, NewLocationZ);
	}
	
	SetNewClimbPosition(HorizontalLocation.X, HorizontalLocation.Y, NewLocationZ, WallRotation);
	DecideClimbStyle(ClimbTopHitResult.ImpactPoint, WallRotation);
}

void UTraversalComponent::ServerSendClimbInput_Implementation(float FValue, float RValue)
{
	ForwardValue = FValue;
	RightValue = RValue;
	ClimbMovement();
}


void UTraversalComponent::StopClimbMovement()
{
	CharacterMovement->StopMovementImmediately();
	SetClimbDirection(UGT.Traversal_Direction_NoDirection);
}

void UTraversalComponent::ServerStopClimbMovement_Implementation()
{
	StopClimbMovement();
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
			GetDebugType(ClimbCheckForSidesDebugType),
			HitResult,
			true,
			ClimbCheckForSidesDebugTraceColor,
			ClimbCheckForSidesDebugHitColor,
			ClimbCheckForSidesDebugTime);

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

bool UTraversalComponent::ValidateClimbMovementSurface(const FVector& MovementImpactLocation) const
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
		ClimbMovementSurfaceValidationCapsuleRadius,
		ClimbMovementSurfaceValidationCapsuleHalfHeight,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		TArray<AActor*>(),
		GetDebugType(ClimbMovementSurfaceValidationDebugType),
		HitResult,
		true,
		ClimbMovementSurfaceValidationDebugTraceColor,
		ClimbMovementSurfaceValidationHitColor,
		ClimbMovementSurfaceValidationDebugTime);

	return !HitResult.bBlockingHit;
}

bool UTraversalComponent::ValidateAirHang() const
{

	if (!NextClimbResult.bBlockingHit)
	{
		return false;
	}

	float ClimbLocationZ = NextClimbResult.ImpactPoint.Z;
	float CharacterHeadZ = SkeletalMesh->GetBoneLocation("Head").Z;
	float HeightDifference = CharacterHeadZ - ClimbLocationZ;
	float Distance = FVector::Distance(NextClimbResult.ImpactPoint, SkeletalMesh->GetBoneLocation("Head"));
	
	//UE_LOG(TraversalComponentLog, Log, TEXT("ValidateAirHang: Distance = [%f]"), Distance);
	
	if (HeightDifference < AirHangMaxHeightDifference || Distance > AirHangMinDistance)
	{
		return false;
	}

	if (bInLand)
	{
		return false;
	}

	return true;
}

/********************* IK **********************/

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
		float MoveValue = bLeftHand ? 
			(WallDetectionMaxIndex - WallDetectionIndex * 2) * -1.f :
			(WallDetectionMaxIndex - WallDetectionIndex * 2) * 1.f;
		
		FVector WallDetectionInitialVector = VectorDirectionMoveWithRotation(
			NextClimbResult.ImpactPoint,
			UGT.Traversal_Direction_Right,
			MoveValue,
			WallRotation);
		
		FVector TraceStart = VectorDirectionMoveWithRotation(
			WallDetectionInitialVector,
			UGT.Traversal_Direction_Backward,
			HandIKTraceDistance, 
			WallRotation);
		
		FVector TraceEnd = VectorDirectionMoveWithRotation(
			WallDetectionInitialVector,
			UGT.Traversal_Direction_Forward,
			HandIKTraceDistance,
			WallRotation);
		
		bool bHit = UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			TraceStart,
			TraceEnd,
			HandIKWallDetectionSphereRadius,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			GetDebugType(HandIKWallDetectionDebugType),
			ClimbWallHitResult,
			true,
			HandIKWallDetectionDebugTraceColor,
			HandIKWallDetectionDebugHitColor,
			HandIKWallDetectionDebugTime);

		if (!ClimbWallHitResult.bBlockingHit)
		{
			continue;
		}
		
		WallRotation = ReverseNormal(ClimbWallHitResult.ImpactNormal);
		ClimbHandRotation = bLeftHand ? WallRotation + HandIKLeftHandAdditiveRotation : WallRotation + HandIKRightHandAdditiveRotation;
		
		for (int32 TopDetectionIndex = 0; TopDetectionIndex < HandIKTopDetectionIterations; TopDetectionIndex++)
		{
			FVector TopDetectionInitialVector = VectorDirectionMoveWithRotation(
				ClimbWallHitResult.ImpactPoint,
				UGT.Traversal_Direction_Forward,
				HandIKTopDetectionForwardOffset,
				WallRotation);
			
			TraceStart = VectorDirectionMove(TopDetectionInitialVector,
				UGT.Traversal_Direction_Up,
				TopDetectionIndex * HandIKTopDetectionVerticalOffsetMultiplier);
			
			TraceEnd = VectorDirectionMove(TopDetectionInitialVector,
				UGT.Traversal_Direction_Down,
				HandIKTopDetectionTraceDistance);
			
			UKismetSystemLibrary::SphereTraceSingle(
				GetWorld(),
				TraceStart,
				TraceEnd,
				HandIKTopDetectionSphereRadius,
				UEngineTypes::ConvertToTraceType(ECC_Visibility),
				false,
				TArray<AActor*>(),
				GetDebugType(HandIKTopDetectionDebugType),
				ClimbTopHitResult,
				true,
				HandIKTopDetectionDebugTraceColor,
				HandIKTopDetectionDebugHitColor,
				HandIKTopDetectionDebugTime);

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

	// UE_LOG(TraversalComponentLog, Log, TEXT("Owner Role = [%d]"), GetOwnerRole());
	// UE_LOG(TraversalComponentLog, Log, TEXT("Next Climb Ik: Hand = [%d]"), bLeftHand);
	// UE_LOG(TraversalComponentLog, Log, TEXT("Next Climb IK: Location = [%s]"), *ClimbHandLocation.ToString());
	// UE_LOG(TraversalComponentLog, Log, TEXT("Next Climb IK: Rotation = [%s]"), *ClimbHandRotation.ToString());
	
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
			GetDebugType(FootIKDebugType),
			ClimbWallHitResult,
			true,
			FootIKDebugTraceColor,
			FootIKDebugHitColor,
			FootIKDebugTime);

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
	
	//UE_LOG(TraversalComponentLog, Log, TEXT("Next Foot IK: Foot Location = [%s]"), *ClimbWallHitResult.ToString());
	
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
			GetDebugType(HandIKUpdateWallDetectionDebugType),
			ClimbWallHitResult,
			true,
			HandIKUpdateWallDetectionDebugTraceColor,
			HandIKUpdateWallDetectionDebugHitColor,
			HandIKUpdateWallDetectionDebugTime);

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
			GetDebugType(HandIKUpdateTopDetectionDebugType),
			ClimbTopHitResult,
			true,
			HandIKUpdateTopDetectionDebugTraceColor,
			HandIKUpdateTopDetectionDebugHitColor,
			HandIKUpdateTopDetectionDebugTime);

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
			GetDebugType(FootIKUpdateWallDetectionDebugType),
			ClimbWallHitResult,
			true,
			FootIKUpdateWallDetectionDebugTraceColor,
			FootIKUpdateWallDetectionDebugHitColor,
			FootIKUpdateWallDetectionDebugTime);

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

void UTraversalComponent::ResetFootIK() const
{
	if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UTraversalInterface::StaticClass()))
	{
		ITraversalInterface::Execute_SetRightFootClimbLocation(AnimInstance, SkeletalMesh->GetSocketLocation("ik_foot_r"));
		ITraversalInterface::Execute_SetLeftFootClimbLocation(AnimInstance, SkeletalMesh->GetSocketLocation("ik_foot_l"));
	}
}

/********************* Mantle **********************/

bool UTraversalComponent::ValidateMantleSurface() const
{

	float CharacterCapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	float CharacterCapsuleRadius = Capsule->GetScaledCapsuleRadius();

	if (WallTopResult.ImpactPoint == FVector::ZeroVector)
	{
		return false;
	}
	
	FVector CapsuleLocation = VectorDirectionMove(
		WallTopResult.ImpactPoint,
		UGT.Traversal_Direction_Up,
		CharacterCapsuleHalfHeight + MantleSurfaceValidationCapsuleVerticalOffset);

	FHitResult HitResult;
	UKismetSystemLibrary::CapsuleTraceSingle(
		this,
		CapsuleLocation,
		CapsuleLocation,
		CharacterCapsuleRadius,
		CharacterCapsuleHalfHeight,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		TArray<AActor*>(),
		GetDebugType(MantleSurfaceValidationDebugType),
		HitResult,
		true,
		MantleSurfaceValidationDebugTraceColor,
		MantleSurfaceValidationDebugHitColor,
		MantleSurfaceValidationDebugTime);

	return !HitResult.bBlockingHit;
}

/********************* Hop **********************/

float UTraversalComponent::GetVerticalHopDistance() const
{
	return DirectionValues(GetControllerDirection(),
		HopForwardDirectionVerticalDistance,
		HopForwardRightDirectionVerticalDistance,
		HopRightDirectionVerticalDistance,
		HopBackwardRightDirectionVerticalDistance,
		HopBackwardDirectionVerticalDistance,
		HopBackwardLeftDirectionVerticalDistance,
		HopLeftDirectionVerticalDistance,
		HopForwardLeftDirectionVerticalDistance) * HopVerticalDistanceMultiplier;
}

float UTraversalComponent::GetHorizontalHopDistance() const
{
	return DirectionValues(GetControllerDirection(),
	HopForwardDirectionHorizontalDistance,
	HopForwardRightDirectionHorizontalDistance,
	HopRightDirectionHorizontalDistance,
	HopBackwardRightDirectionHorizontalDistance,
	HopBackwardDirectionHorizontalDistance,
	HopBackwardLeftHorizontalDistance,
	HopLeftDirectionHorizontalDistance,
	HopForwardLeftDirectionHorizontalDistance) * HopHorizontalDistanceMultiplier; 
}

FGameplayTag UTraversalComponent::DecideHopAction() const
{
	FGameplayTag Direction = GetControllerDirection();

	if (bool bBraced = ClimbStyle.MatchesTagExact(UGT.Traversal_ClimbStyle_BracedClimb))
	{
		if (Direction.MatchesTagExact(UGT.Traversal_Direction_Forward))
		{
			return UGT.Traversal_Action_BracedClimb_HopUp;
		}
		if (Direction.MatchesTagExact(UGT.Traversal_Direction_ForwardRight))
		{
			return UGT.Traversal_Action_BracedClimb_HopRightUp;
		}
		if (Direction.MatchesTagExact(UGT.Traversal_Direction_Right))
		{
			return UGT.Traversal_Action_BracedClimb_HopRight;
		}
		if (Direction.MatchesTagExact(UGT.Traversal_Direction_Backward))
		{
			return UGT.Traversal_Action_BracedClimb_HopDown;
		}
		if (Direction.MatchesTagExact(UGT.Traversal_Direction_BackwardLeft))
		{
			return UGT.Traversal_Action_BracedClimb_HopLeft;
		}
		if (Direction.MatchesTagExact(UGT.Traversal_Direction_Left))
		{
			return UGT.Traversal_Action_BracedClimb_HopLeft;
		}
		if (Direction.MatchesTagExact(UGT.Traversal_Direction_ForwardLeft))
		{
			return UGT.Traversal_Action_BracedClimb_HopLeftUp;
		}
	}
	else
	{
		if (Direction.MatchesTagExact(UGT.Traversal_Direction_Forward))
		{
			return UGT.Traversal_Action_FreeHang_HopUp;
		}
		if (Direction.MatchesTagExact(UGT.Traversal_Direction_ForwardRight))
		{
			return UGT.Traversal_Action_FreeHang_HopRightUp;
		}
		if (Direction.MatchesTagExact(UGT.Traversal_Direction_Right))
		{
			return UGT.Traversal_Action_FreeHang_HopRight;
		}
		if (Direction.MatchesTagExact(UGT.Traversal_Direction_Backward))
		{
			return UGT.Traversal_Action_FreeHang_HopDown;
		}
		if (Direction.MatchesTagExact(UGT.Traversal_Direction_BackwardLeft))
		{
			return UGT.Traversal_Action_FreeHang_HopLeft;
		}
		if (Direction.MatchesTagExact(UGT.Traversal_Direction_Left))
		{
			return UGT.Traversal_Action_FreeHang_HopLeft;
		}
		if (Direction.MatchesTagExact(UGT.Traversal_Direction_ForwardLeft))
		{
			return UGT.Traversal_Action_FreeHang_HopLeftUp;
		}
	}

	return UGT.Traversal_Action_BracedClimb_HopUp;
}

void UTraversalComponent::FindHopLocation()
{
	float VerticalHopDistance = GetVerticalHopDistance();
	float HorizontalHopDistance = GetHorizontalHopDistance();

	FVector VerticalOffset = VectorDirectionMove(
		WallTopResult.ImpactPoint,
		UGT.Traversal_Direction_Up,
		VerticalHopDistance);

	FVector ScanLocation = VectorDirectionMoveWithRotation(
		VerticalOffset,
		UGT.Traversal_Direction_Right,
		HorizontalHopDistance,
		OwnerCharacter->GetActorRotation());

	//UE_LOG(TraversalComponentLog, Log, TEXT("Find Hop Location: Grid Scan Location = [%s]"), *ScanLocation.ToString());
	GridScan(HopGridScanWidth, HopGridScanHeight, ScanLocation, WallRotation);
}

/********************* Vault **********************/

bool UTraversalComponent::ValidateVaultSurface() const
{
	float OnObstacleCapsuleTraceHeight = Capsule->GetScaledCapsuleHalfHeight() / 2.f;
	float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
	
	FVector OnObstacleCapsuleTraceLocation = VectorDirectionMove(
		WallTopResult.ImpactPoint,
		UGT.Traversal_Direction_Up,
		OnObstacleCapsuleTraceHeight + VaultOnObstacleCapsuleTraceVerticalOffset);

	FHitResult OnObstacleHitResult;
	UKismetSystemLibrary::CapsuleTraceSingle(
		this,
		OnObstacleCapsuleTraceLocation,
		OnObstacleCapsuleTraceLocation,
		CapsuleRadius,
		OnObstacleCapsuleTraceHeight,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		TArray<AActor*>(),
		GetDebugType(VaultOnObstacleSurfaceValidationDebugType),
		OnObstacleHitResult,
		true,
		VaultOnObstacleSurfaceValidationDebugTraceColor,
		VaultOnObstacleSurfaceValidationDebugHitColor,
		VaultOnObstacleSurfaceValidationDebugTime);

	float VaultCapsuleTraceHeight = Capsule->GetScaledCapsuleHalfHeight();

	FVector VaultCapsuleLocation = VectorDirectionMove(
		WallVaultResult.ImpactPoint,
		UGT.Traversal_Direction_Up,
		VaultCapsuleTraceHeight + VaultCapsuleTraceVerticalOffset);

	FHitResult VaultHitResult;
	UKismetSystemLibrary::CapsuleTraceSingle(
		this,
		VaultCapsuleLocation,
		VaultCapsuleLocation,
		CapsuleRadius,
		VaultCapsuleTraceHeight,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		TArray<AActor*>(),
		GetDebugType(VaultSurfaceValidationDebugType),
		VaultHitResult,
		true,
		VaultSurfaceValidationDebugTraceColor,
		VaultSurfaceValidationDebugHitColor,
		VaultSurfaceValidationDebugTime);

	return !OnObstacleHitResult.bBlockingHit && !VaultHitResult.bBlockingHit;
}

/********************* Drop **********************/

void UTraversalComponent::DropFromClimb()
{
	if (bInLand)
	{
		return;
	}

	if (TraversalState.MatchesTagExact(UGT.Traversal_State_Climb))
	{
		SetTraversalState(UGT.Traversal_State_FreeRoam);
		bIsDropping = true;
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			bIsDropping = false;
		}, IsDroppingResetTime, false);
	}
}

void UTraversalComponent::ServerDropFromClimb_Implementation()
{
	DropFromClimb();
}

void UTraversalComponent::ServerTriggerTraversalAction_Implementation(bool bJumpAction)
{
	TriggerTraversalAction(bJumpAction);
}

/********************* Helpers **********************/

void UTraversalComponent::ResetMovement()
{
	ForwardValue = 0.f;
	RightValue = 0.f;
	SetClimbDirection(UGT.Traversal_Direction_NoDirection);

	if (TraversalAction.MatchesTagExact(UGT.Traversal_Action_NoAction) &&
		TraversalState.MatchesTagExact(UGT.Traversal_State_Climb))
	{
		CharacterMovement->StopMovementImmediately();
	}
}

void UTraversalComponent::ServerResetMovement_Implementation()
{
	ResetMovement();
}

FVector UTraversalComponent::VectorDirectionMove(const FVector& Source, const FGameplayTag& Direction, const float& MoveValue) const
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
	const float& MoveValue, const FRotator& Rotation) const
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

FGameplayTag UTraversalComponent::GetControllerDirection() const
{
	//UE_LOG(LogTemp, Log, TEXT("FD = [%f] | RD = [%f]"), ForwardValue, RightValue)

	FVector2D InputVector(RightValue, ForwardValue);

	if (InputVector.SizeSquared() < 0.1f * 0.1f)
	{
		return UGT.Traversal_Direction_NoDirection;
	}
	
	InputVector.Normalize();

	float Angle = FMath::RadiansToDegrees(FMath::Atan2(InputVector.X, InputVector.Y));

	// Разбиение по 8 направлениям, по 45 градусов
	if (Angle >= -22.5f && Angle <= 22.5f)
		return UGT.Traversal_Direction_Forward;
	if (Angle > 22.5f && Angle <= 67.5f)
		return UGT.Traversal_Direction_ForwardRight;
	if (Angle > 67.5f && Angle <= 112.5f)
		return UGT.Traversal_Direction_Right;
	if (Angle > 112.5f && Angle <= 157.5f)
		return UGT.Traversal_Direction_BackwardRight;
	if (Angle > 157.5f || Angle < -157.5f)
		return UGT.Traversal_Direction_Backward;
	if (Angle > -157.5f && Angle <= -112.5f)
		return UGT.Traversal_Direction_BackwardLeft;
	if (Angle > -112.5f && Angle <= -67.5f)
		return UGT.Traversal_Direction_Left;
	if (Angle > -67.5f && Angle <= -22.5f)
		return UGT.Traversal_Direction_ForwardLeft;

	return UGT.Traversal_Direction_NoDirection;
}

float UTraversalComponent::GetCharacterHandHeight() const
{
	return FMath::Max(
		SkeletalMesh->GetBoneLocation("hand_l").Z,
		SkeletalMesh->GetBoneLocation("hand_r").Z);
}

EDrawDebugTrace::Type UTraversalComponent::GetDebugType(ETraversalDebugType DebugValue) const
{
	switch (DebugValue)
	{
	case ETraversalDebugType::None:
		return EDrawDebugTrace::Type::None;
	case ETraversalDebugType::ForOneFrame:
		return EDrawDebugTrace::ForOneFrame;
	case ETraversalDebugType::ForDuration:
		return EDrawDebugTrace::ForDuration;
	case ETraversalDebugType::Persistent:
		return EDrawDebugTrace::Persistent;
	default:
		return EDrawDebugTrace::Type::None;	
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
}

float UTraversalComponent::ClimbValues(const FGameplayTag& NewClimbStyle, float Braced, float FreeHang) const
{
	return ClimbStyle.MatchesTagExact(FUmbraGameplayTags::Get().Traversal_ClimbStyle_BracedClimb) ? Braced : FreeHang;
}

float UTraversalComponent::DirectionValues(const FGameplayTag& Direction, float Forward, float ForwardRight, float Right,
		float BackwardRight, float Backward, float BackwardLeft, float Left, float ForwardLeft) const
{
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Forward))
	{
		return Forward;
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_ForwardRight))
	{
		return ForwardRight;
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Right))
	{
		return Right;
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_BackwardRight))
	{
		return BackwardRight;
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Backward))
	{
		return Backward;
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_BackwardLeft))
	{
		return BackwardLeft;
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_Left))
	{
		return Left;
	}
	if (Direction.MatchesTagExact(UGT.Traversal_Direction_ForwardLeft))
	{
		return ForwardLeft;
	}
	return Forward;
}

float UTraversalComponent::TraversalStateValues(const FGameplayTag& State, float FreeRoam, float ReadyToClimb,
	float Climb, float Mantle, float Vault) const
{
	if (State.MatchesTagExact(UGT.Traversal_State_FreeRoam))
	{
		return FreeRoam;
	}
	if (State.MatchesTagExact(UGT.Traversal_State_ReadyToClimb))
	{
		return ReadyToClimb;
	}
	if (State.MatchesTagExact(UGT.Traversal_State_Climb))
	{
		return Climb;
	}
	if (State.MatchesTagExact(UGT.Traversal_State_Mantle))
	{
		return Mantle;
	}
	if (State.MatchesTagExact(UGT.Traversal_State_Vault))
	{
		return Vault;
	}

	return FreeRoam;
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


