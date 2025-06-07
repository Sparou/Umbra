// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Actor/Traps/UmbraTrap.h"
#include "UmbraTeleportTrap.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API AUmbraTeleportTrap : public AUmbraTrap
{
	GENERATED_BODY()

	// void Tick(float DeltaTime);
	
protected:

	UPROPERTY(EditAnywhere)
	float TeleportDistance = 100.f;
	
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};