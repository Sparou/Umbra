#include "Stealth/DayNightCycleManager.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Stealth/CandleFlickerComponent.h"

ADayNightCycleManager::ADayNightCycleManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADayNightCycleManager::BeginPlay()
{
	Super::BeginPlay();
	CollectAllNightLights();
}

void ADayNightCycleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentTime += DeltaTime;
	float NormalizedTime = FMath::Fmod(CurrentTime, DayLengthInSeconds) / DayLengthInSeconds;
	UpdateLighting(NormalizedTime);

	float SunAngle = NormalizedTime * 360.f - 90.f;
	float SunIntensity = FMath::Clamp(FMath::Sin(FMath::DegreesToRadians(SunAngle)), 0.f, 1.f);
	UpdateNightLights(SunIntensity, DeltaTime);
}

void ADayNightCycleManager::UpdateLighting(float NormalizedTime)
{
	if (!SunLight || !SkyLight || !SkySphereActor) return;

	float PitchAngle = NormalizedTime * 360.f - 90.f;
	FRotator NewRotation = FRotator(-PitchAngle, 0.f, 0.f);
	SunLight->GetLightComponent()->SetWorldRotation(NewRotation);

	float SunIntensity = FMath::Clamp(FMath::Sin(FMath::DegreesToRadians(PitchAngle)), 0.f, 1.f);
	SunLight->GetLightComponent()->SetIntensity(SunIntensity * 10.0f);

	if (USkyLightComponent* SkyComp = SkyLight->GetLightComponent())
	{
		SkyComp->SetIntensity(SunIntensity * 3.0f);
		SkyComp->MarkRenderStateDirty();
	}

	UFunction* UpdateFunc = SkySphereActor->FindFunction(FName("UpdateSunDirection"));
	if (UpdateFunc)
	{
		SkySphereActor->ProcessEvent(UpdateFunc, nullptr);
	}
}

void ADayNightCycleManager::CollectAllNightLights()
{
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

	for (AActor* Actor : AllActors)
	{
		TArray<ULightComponent*> LightComponents;
		Actor->GetComponents(LightComponents);

		for (ULightComponent* LightComp : LightComponents)
		{
			if (Cast<UPointLightComponent>(LightComp) || Cast<USpotLightComponent>(LightComp))
			{
				ToggleableLights.Add(FManagedLight(LightComp));
			}
		}

		TArray<UCandleFlickerComponent*> FlickerComponents;
		Actor->GetComponents(FlickerComponents);
		CandleFlickers.Append(FlickerComponents);
	}
}

void ADayNightCycleManager::UpdateNightLights(float SunIntensity, float DeltaTime)
{
	const float TargetFactor = (SunIntensity < NightThreshold) ? 1.f : 0.f;

	for (FManagedLight& Managed : ToggleableLights)
	{
		if (!Managed.Light) continue;

		float Current = Managed.Light->Intensity;
		float Target = Managed.BaseIntensity * TargetFactor;
		float NewValue = FMath::FInterpTo(Current, Target, DeltaTime, FadeSpeed);
		Managed.Light->SetIntensity(NewValue);
	}

	for (UCandleFlickerComponent* Flicker : CandleFlickers)
	{
		if (!Flicker) continue;

		bool bShouldTick = (TargetFactor >= 0.5f);
		Flicker->SetComponentTickEnabled(bShouldTick);
	}
}
