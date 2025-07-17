// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "LightSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(UmbraLightSubsystemLog, Log, All);

/**
 * 
 */
UCLASS()
class UMBRA_API ULightSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	
	void RegisterLightSource(AActor* LightActor);
	void UnregisterLightSource(AActor* LightActor);

	TArray<TWeakObjectPtr<AActor>> GetLightSourcesInRadius(const FVector& Origin, float Radius);

protected:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:

	TArray<TWeakObjectPtr<AActor>> StealthAffectingLights;
};
