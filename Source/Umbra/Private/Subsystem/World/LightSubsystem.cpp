// Copyrighted by Vorona Games

#include "Subsystem/World/LightSubsystem.h"

DEFINE_LOG_CATEGORY(UmbraLightSubsystemLog)

void ULightSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(UmbraLightSubsystemLog, Log, TEXT("Light Subsystem Initialized"));
}

void ULightSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(UmbraLightSubsystemLog, Log, TEXT("Light Subsystem Deinitialized"));
	
}

void ULightSubsystem::RegisterLightSource(AActor* LightActor)
{
	if (LightActor == nullptr) return;
	StealthAffectingLights.Add(LightActor);
}

void ULightSubsystem::UnregisterLightSource(AActor* LightActor)
{
	if (LightActor == nullptr) return;
	StealthAffectingLights.Remove(LightActor);
}

TArray<TWeakObjectPtr<AActor>> ULightSubsystem::GetLightSourcesInRadius(const FVector& Origin, float Radius)
{
	TArray<TWeakObjectPtr<AActor>> NearbyLights;
	const float RadiusSquared = Radius * Radius;

	for (const auto& WeakLight : StealthAffectingLights)
	{
		if (WeakLight.IsValid())
		{
			if (FVector::DistSquared(Origin, WeakLight->GetActorLocation()) <= RadiusSquared)
			{
				NearbyLights.Add(WeakLight);
			}
		}
	}

	return NearbyLights;
}