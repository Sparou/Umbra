// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraPortalActor.generated.h"

class UCapsuleComponent;

UCLASS()
class UMBRA_API AUmbraPortalActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUmbraPortalActor();

	virtual void BeginPlay() override;
	void Init(FVector TrgLocation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetLocation;

protected:
	UPROPERTY(EditAnywhere)
	float LifeSpan = 10.0f;
	
	UPROPERTY(EditAnywhere)
	UCapsuleComponent* PortalCollision;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	
	static TArray<AActor*> RecentlyTeleportedActors;
};
