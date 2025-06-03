// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "UmbraBaseProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class UMBRA_API AUmbraBaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	AUmbraBaseProjectile();

	TObjectPtr<USphereComponent> GetSphereComponent() const
	{
		return SphereComponent;
	}

	TObjectPtr<UProjectileMovementComponent> GetProjectileMovementComponent() const
	{
		return ProjectileMovementComponent;
	}

	void SetGameplayEffectSpecHandle(const FGameplayEffectSpecHandle& NewGameplayEffectSpecHandle)
	{
		GameplayEffectSpecHandle = NewGameplayEffectSpecHandle;
	}

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile)
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile, meta = (ClampMin = 0))
	float LifeSpan;

private:
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FGameplayEffectSpecHandle GameplayEffectSpecHandle;
};
