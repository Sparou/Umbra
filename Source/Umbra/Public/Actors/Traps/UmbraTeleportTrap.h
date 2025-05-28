// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Actors/Traps/UmbraTrap.h"
#include "UmbraTeleportTrap.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API AUmbraTeleportTrap : public AUmbraTrap
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere)
	float TeleportDistance = 100.f;
	
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};