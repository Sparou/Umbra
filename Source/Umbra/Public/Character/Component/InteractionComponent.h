// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(InteractionComponentLog, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMBRA_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UInteractionComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetInteractionActor() const;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionDistance = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionTraceSphereRadius = 4.f;
	
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	
	void InteractionTrace();
	void HandleHit();
	void HandleNoHit();

	UFUNCTION(Server, Reliable)
	void ServerSetInteractionActor(AActor* NewInteractionActor);
	
	FHitResult InteractionResult;
	FVector InteractionTraceStart;
	FVector InteractionTraceEnd;
	
	AActor* InteractionActor;
	
	UPROPERTY()
	TObjectPtr<APlayerCameraManager> CameraManager;

};
