// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraTrap.generated.h"

class UGameplayEffect;
class AUmbraPlayerCharacter;
class USphereComponent;

UCLASS()
class UMBRA_API AUmbraTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUmbraTrap();

	// Sets default values for this actor's properties
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Category = "Trap")
	AUmbraPlayerCharacter* InitializedCharacter;
	
protected:
	UPROPERTY(EditAnywhere)
	float LifeSpan = 10.0f;
	
	UPROPERTY(EditAnywhere)
	USphereComponent* SphereCollision;

	UFUNCTION()
	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "Trap")
	TSubclassOf<UGameplayEffect> Effect;
};