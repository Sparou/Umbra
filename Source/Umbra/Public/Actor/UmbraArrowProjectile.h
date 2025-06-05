// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraArrowProjectile.generated.h"

class UProjectileMovementComponent;
class UGameplayEffect;

UCLASS()
class UMBRA_API AUmbraArrowProjectile : public APawn
{
	GENERATED_BODY()
	
public:
	AUmbraArrowProjectile();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* Movement;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> HitEffect;
	
	UPROPERTY(EditAnywhere)
	float MinSpeed = 10.f;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
			   UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};