#include "Stealth/DayNightCycleManager.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Stealth/CandleFlickerComponent.h"
#include "Stealth/UMaterialSwitcherComponent.h"

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
	float SunFactor = FMath::Clamp(FMath::Sin(FMath::DegreesToRadians(SunAngle)), 0.f, 1.f);

	UpdateNightLights(SunFactor, DeltaTime);

	bool bNowNight = SunFactor < NightThreshold;
	if (bNowNight != bIsNight)
	{
		bIsNight = bNowNight;
		UpdateNightActors(bIsNight);
		UpdateMaterialSwitchers(bIsNight);
	}
}

void ADayNightCycleManager::UpdateLighting(float NormalizedTime)
{
	if (!SunLight || !SkyLight || !SkySphereActor) return;

	float PitchAngle = NormalizedTime * 360.f - 90.f;
	FRotator NewSunRotation = FRotator(-PitchAngle, 0.f, 0.f);
	SunLight->GetLightComponent()->SetWorldRotation(NewSunRotation);

	float SunFactor = FMath::Clamp(FMath::Sin(FMath::DegreesToRadians(PitchAngle)), 0.f, 1.f);

	// Солнце
	float MinSunIntensity = 0.0f;
	float MaxSunIntensity = 10.0f;
	float FinalSunIntensity = FMath::Lerp(MinSunIntensity, MaxSunIntensity, SunFactor);
	SunLight->GetLightComponent()->SetIntensity(FinalSunIntensity);

	// Цвета солнца
	FLinearColor NightColor(0.1f, 0.15f, 0.3f);
	FLinearColor DawnColor(1.0f, 0.4f, 0.2f);
	FLinearColor DayColor(1.0f, 1.0f, 0.9f);

	FLinearColor FinalColor;
	if (SunFactor < 0.3f)
	{
		FinalColor = FLinearColor::LerpUsingHSV(NightColor, DawnColor, SunFactor / 0.3f);
	}
	else
	{
		FinalColor = FLinearColor::LerpUsingHSV(DawnColor, DayColor, (SunFactor - 0.3f) / 0.7f);
	}

	SunLight->GetLightComponent()->SetLightColor(FinalColor);

	// Луна (если задана)
	if (MoonLight)
	{
		// Противоположный угол
		FRotator MoonRotation = FRotator(-(PitchAngle + 180.f), 0.f, 0.f);
		MoonLight->GetLightComponent()->SetWorldRotation(MoonRotation);

		float MoonIntensity = FMath::Lerp(0.25f, 0.0f, SunFactor); // Яркая при ночи, исчезает при дне
		MoonLight->GetLightComponent()->SetIntensity(MoonIntensity);

		FLinearColor MoonColor = FLinearColor(0.4f, 0.5f, 1.0f); // Холодный голубой цвет луны
		MoonLight->GetLightComponent()->SetLightColor(MoonColor);
	}

	// SkyLight
	if (USkyLightComponent* SkyComp = SkyLight->GetLightComponent())
	{
		SkyComp->SetIntensity(FMath::Lerp(0.5f, 3.0f, SunFactor));
		SkyComp->SetLightColor(FinalColor);
		SkyComp->MarkRenderStateDirty();
	}

	// SkySphere
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

		TArray<UMaterialSwitcherComponent*> MaterialComps;
		Actor->GetComponents(MaterialComps);
		MaterialSwitchers.Append(MaterialComps);
	}
}

void ADayNightCycleManager::UpdateNightLights(float SunFactor, float DeltaTime)
{
	const float TargetFactor = (SunFactor < NightThreshold) ? 3.f : 0.f;

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
		Flicker->SetComponentTickEnabled(TargetFactor >= 0.5f);
	}
}

void ADayNightCycleManager::UpdateNightActors(bool bEnable)
{
	for (AActor* Actor : NightActiveActors)
	{
		if (!Actor) continue;
		Actor->SetActorHiddenInGame(!bEnable);
		Actor->SetActorEnableCollision(bEnable);
		Actor->SetActorTickEnabled(bEnable);
	}
}

void ADayNightCycleManager::UpdateMaterialSwitchers(bool bNight)
{
	for (UMaterialSwitcherComponent* Switcher : MaterialSwitchers)
	{
		if (!Switcher) continue;
		Switcher->SetIsNight(bNight);
	}
}
