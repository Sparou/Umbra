// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraLightSource.generated.h"

UCLASS()
class UMBRA_API AUmbraLightSource : public AActor
{
	GENERATED_BODY()
	
public:	
	AUmbraLightSource();
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
};
