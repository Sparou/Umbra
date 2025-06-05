// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundMix.h"
#include "UmbraSoundDome.generated.h"

class USphereComponent;

UCLASS()
class UMBRA_API AUmbraSoundDome : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUmbraSoundDome();

	UPROPERTY(EditAnywhere)
	USoundMix* SilenceMix;
	
	UPROPERTY(EditAnywhere)
	USphereComponent* DomeCollision;

	UFUNCTION()
	void Init(float Radius, float LifeSpan);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);
};
