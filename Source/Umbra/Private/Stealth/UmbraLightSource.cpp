// Copyrighted by Vorona Games


#include "Stealth/UmbraLightSource.h"
#include "Subsystem/World/LightSubsystem.h"

AUmbraLightSource::AUmbraLightSource()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AUmbraLightSource::BeginPlay()
{
	Super::BeginPlay();
	if (UWorld* World = GetWorld())
	{
		if (ULightSubsystem* LightSubsystem = World->GetSubsystem<ULightSubsystem>())
		{
			LightSubsystem->RegisterLightSource(this);
		}
	}
}

void AUmbraLightSource::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (UWorld* World = GetWorld())
	{
		if (ULightSubsystem* LightSubsystem = World->GetSubsystem<ULightSubsystem>())
		{
			LightSubsystem->UnregisterLightSource(this);
		}
	}
}

