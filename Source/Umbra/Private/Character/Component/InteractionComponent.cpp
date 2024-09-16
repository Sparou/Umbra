// Copyrighted by Vorona Games


#include "Character/Component/InteractionComponent.h"

#include "Components/BoxComponent.h"


UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner())
	{
		InteractionCollisionComponent = GetOwner()->GetComponentByClass<UBoxComponent>();

		checkf(InteractionCollisionComponent, TEXT("Interaction Collision Component is not added to [%s]"), *GetOwner()->GetName());
		
		InteractionCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &UInteractionComponent::OnBeginOverlap);
		InteractionCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &UInteractionComponent::OnEndOverlap);
	}
}


void UInteractionComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OverlappedActor = OtherActor;
}

void UInteractionComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappedActor = nullptr;
}
