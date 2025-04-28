// Copyrighted by Vorona Games

#include "Character/Component/InteractionComponent.h"
#include "Interaction/InteractionInterface.h"
#include "Kismet/GameplayStatics.h"

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
	InteractionTrace();
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	checkf(CameraManager, TEXT("Interaction Component: Camera Manager initialization failed!"));
}

void UInteractionComponent::InteractionTrace()
{
	InteractionTraceStart = CameraManager->GetCameraLocation();
	InteractionTraceEnd = InteractionTraceStart + CameraManager->GetActorForwardVector() * InteractionDistance;
  
	UKismetSystemLibrary::SphereTraceSingle(
	  this,
	  InteractionTraceStart,
	  InteractionTraceEnd,
	  InteractionTraceSphereRadius,
	  UEngineTypes::ConvertToTraceType(ECC_Visibility),
	  false,
	  TArray<AActor*>(),
	  EDrawDebugTrace::ForOneFrame,
	  InteractionResult,
	  true);
  
	if (!InteractionResult.bBlockingHit || !InteractionResult.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		InteractionActor = nullptr;
		return;
	}

	if (InteractionActor != InteractionResult.GetActor())
	{
		UE_LOG(InteractionComponentLog, Log, TEXT("Interaction Component: Target = [%s]"), *InteractionResult.GetActor()->GetName());
	}
	InteractionActor = InteractionResult.GetActor();
}
