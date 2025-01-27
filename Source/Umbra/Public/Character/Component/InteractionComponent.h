// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMBRA_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponent();
	AActor* GetCurrentTarget() const { return Target; }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionDistance = 1000.f;
	
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void BeginPlay() override;

private:

	TObjectPtr<AActor> Owner;
	TObjectPtr<AActor> Target;
	
	void CheckForTarget(const FVector& TraceStart, const FVector&  TraceEnd);

};
