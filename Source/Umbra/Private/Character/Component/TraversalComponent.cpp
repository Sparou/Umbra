// Copyrighted by Vorona Games


#include "Character/Component/TraversalComponent.h"
#include "UmbraGameplayTags.h"
#include "Character/Data/ObstacleDetectionSettings.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(UmbraTraversalComponentLog)

UTraversalComponent::UTraversalComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UTraversalComponent::BeginPlay()
{
	Super::BeginPlay();

	UGT = FUmbraGameplayTags::Get();
	OwnerCharacter = Cast<ACharacter>(GetOwner());

}

void UTraversalComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	DetectObstacle();
}

bool UTraversalComponent::DetectObstacle()
{
	UWorld* World = GetWorld();

	if (!World || !OwnerCharacter.IsValid())
	{
		return false;
	}
	
	const FVector OverlapLocation = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * ObstacleDetectionSettings->ObstacleDetectionOffset;
	const FCollisionShape Shape = FCollisionShape::MakeBox(ObstacleDetectionSettings->ObstacleDetectionBoxHalfSize);

	TArray<FOverlapResult> Overlaps;
	const bool bHasOverlap = World->OverlapMultiByChannel(
		Overlaps,
		OverlapLocation,
		OwnerCharacter->GetActorQuat(),
		ECC_Visibility,
		Shape);

#if ENABLE_DRAW_DEBUG
	UKismetSystemLibrary::DrawDebugBox(
		World,
		OverlapLocation,
		ObstacleDetectionSettings->ObstacleDetectionBoxHalfSize,
		FLinearColor::Green,
		OwnerCharacter->GetActorRotation(),
		0.f,
		0.f
	);
#endif

	FName ObstacleTag;
	FindTraversableActorsInOverlaps(ObstacleTag, Overlaps);

	if (ObstacleTag == NAME_None)
	{
		return false;
	}

	return true;
}


void UTraversalComponent::FindTraversableActorsInOverlaps(FName& InName, const TArray<FOverlapResult>& OverlapResults)
{
	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		if (const AActor* const OverlapActor = OverlapResult.GetActor())
		{
			if (OverlapActor->ActorHasTag(ClimbObstacleTagName))
			{
				InName = ClimbObstacleTagName;
				return;
			}
			if (OverlapActor->ActorHasTag(MantleObstacleTagName))
			{
				InName = MantleObstacleTagName;
				return;
			}
			if (OverlapActor->ActorHasTag(VaultObstacleTagName))
			{
				InName = VaultObstacleTagName;
				return;
			}
		}
	}

	InName = NAME_None;
}


FVector UTraversalComponent::MoveVector(const FVector& Source, float Offset, const FGameplayTag& DirectionTag, const FRotator& Rotator) const
{
	FRotationMatrix RotationMatrix = Rotator == FRotator::ZeroRotator ? FRotationMatrix(GetOwner()->GetActorRotation()) : FRotationMatrix(Rotator);

	if (DirectionTag.MatchesTagExact(UGT.Direction_X_Forward))
	{
		return Source * Offset * RotationMatrix.GetScaledAxis(EAxis::X);
	}
	if (DirectionTag.MatchesTagExact(UGT.Direction_X_Backward))
	{
		return Source - Offset * RotationMatrix.GetScaledAxis(EAxis::X);
	}
	if (DirectionTag.MatchesTagExact(UGT.Direction_Y_Right))
	{
		return Source + Offset * RotationMatrix.GetScaledAxis(EAxis::Y);
	}
	if (DirectionTag.MatchesTagExact(UGT.Direction_Y_Left))
	{
		return Source - Offset * RotationMatrix.GetScaledAxis(EAxis::Y);
	}
	if (DirectionTag.MatchesTagExact(UGT.Direction_Z_Up))
	{
		return Source + FVector(0, 0, Offset);
	}
	if (DirectionTag.MatchesTagExact(UGT.Direction_Z_Down))
	{
		return Source - FVector(0, 0, Offset);
	}

	UE_LOG(UmbraTraversalComponentLog, Error, TEXT("Invalid direction tag in MoveVector call from [%s]"), *GetNameSafe(this));
	return Source;
}



