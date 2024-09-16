// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


class IInteractionInterface;
class UBoxComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMBRA_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponent();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components");
	TObjectPtr<UBoxComponent> InteractionCollisionComponent;
	
	UPROPERTY(BlueprintReadOnly)
	AActor* OverlappedActor = nullptr;

protected:
	void BeginPlay() override;
	

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
