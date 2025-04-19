// Copyrighted by Vorona Games


#include "Character/Component/InteractionComponent.h"

#include "Camera/CameraComponent.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	FVector TraceStart;
	FRotator TraceRotation;
	Owner->GetActorEyesViewPoint(TraceStart, TraceRotation);
	FVector TraceEnd = TraceStart + (TraceRotation.Vector() * InteractionDistance);
	CheckForTarget(TraceStart, TraceEnd);
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();
}

void UInteractionComponent::CheckForTarget(const FVector& TraceStart, const FVector& TraceEnd)
{
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_WorldDynamic,
		Params
	);

#if WITH_EDITOR
	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Yellow);
#endif
	
	if (bHit && HitResult.GetActor())
	{
		Target = HitResult.GetActor();
	}
	else
	{
		Target = nullptr;
	}
}
