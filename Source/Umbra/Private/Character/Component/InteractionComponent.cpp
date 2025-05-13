// Copyrighted by Vorona Games

#include "Character/Component/InteractionComponent.h"

#include "UmbraCollisionChannels.h"
#include "Interaction/InteractionInterface.h"
#include "Interface/OutlineInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(InteractionComponentLog)

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

AActor* UInteractionComponent::GetInteractionActor() const
{
	return InteractionActor;
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}
	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		return;
	}
	
	InteractionTrace();
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	checkf(CameraManager, TEXT("Interaction Component: Camera Manager initialization failed!"));
}

void UInteractionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	//DOREPLIFETIME(UInteractionComponent, InteractionActor);
}

void UInteractionComponent::InteractionTrace()
{
	InteractionTraceStart = CameraManager->GetCameraLocation();
	InteractionTraceEnd = InteractionTraceStart + CameraManager->GetActorForwardVector() * InteractionDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.bReturnPhysicalMaterial = false;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractionTraceSphereRadius);
	
	GetWorld()->SweepSingleByChannel(
			InteractionResult,
			InteractionTraceStart,
			InteractionTraceEnd,
			FQuat::Identity,
			ECC_Interaction,
			Sphere,
			Params);
	
	if (!InteractionResult.bBlockingHit ||
		!InteractionResult.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		HandleNoHit();
		return;
	}
		
	HandleHit();
}

void UInteractionComponent::HandleHit()
{
	if (InteractionActor != InteractionResult.GetActor())
	{
		UE_LOG(InteractionComponentLog, Log, TEXT("New Target = [%s]"), *InteractionResult.GetActor()->GetName());
		if (InteractionResult.GetActor()->GetClass()->ImplementsInterface(UOutlineInterface::StaticClass()))
		{
			IOutlineInterface::Execute_EnableOutline(InteractionResult.GetActor(), 1);
		}

		if (InteractionActor)
		{
			IOutlineInterface::Execute_DisableOutline(InteractionActor);
		}
	}

	if (GetOwnerRole() != ROLE_Authority)
	{
		ServerSetInteractionActor(InteractionResult.GetActor());
	}
	
	InteractionActor = InteractionResult.GetActor();
}

void UInteractionComponent::HandleNoHit()
{
	if (InteractionActor && InteractionActor->GetClass()->ImplementsInterface(UOutlineInterface::StaticClass()))
	{
		IOutlineInterface::Execute_DisableOutline(InteractionActor);
	}
	
	if (GetOwnerRole() != ROLE_Authority)
	{
		ServerSetInteractionActor(nullptr);
	}
	
	InteractionActor = nullptr;
}

void UInteractionComponent::ServerSetInteractionActor_Implementation(AActor* NewInteractionActor)
{
	InteractionActor = NewInteractionActor;
}
