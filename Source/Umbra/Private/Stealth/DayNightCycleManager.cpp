#include "Stealth/DayNightCycleManager.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/DirectionalLightComponent.h"
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

	CurrentTime = StartTimeOfDay;
	CollectAllNightLights();
}

void ADayNightCycleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentTime += DeltaTime;
	float Normalized = FMath::Fmod(CurrentTime, DayLengthInSeconds) / DayLengthInSeconds;

	UpdateLighting(Normalized);

	float SunAngle = Normalized * 360.f - 90.f;
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
	if (!SunLight || !SkyLight) return;

	// Поворот солнца
	float SunPitch = NormalizedTime * 360.f - 90.f;
	SunLight->SetActorRotation(FRotator(-SunPitch, 0, 0));

	float SunFactor = FMath::Clamp(FMath::Sin(FMath::DegreesToRadians(SunPitch)), 0.f, 1.f);

	// Солнце
	ULightComponent* SunComp = SunLight->GetLightComponent();
	SunComp->SetIntensity(FMath::Lerp(0.f, SunMaxIntensity, SunFactor));

	FLinearColor SunsetColor;
	if (SunFactor < 0.3f)
	{
		// Плавный переход от ночного (тёмно-синий) к оранжевому через синие оттенки
		float T = SunFactor / 0.3f;
		SunsetColor = FLinearColor::LerpUsingHSV(NightColor, FLinearColor(1.f, 0.55f, 0.25f), T); // Тёмно-синий → оранжевый
	}
	else
	{
		float T = (SunFactor - 0.3f) / 0.7f;
		SunsetColor = FLinearColor::LerpUsingHSV(FLinearColor(1.f, 0.55f, 0.25f), DayColor, T); // Оранжевый → дневной
	}
	SunComp->SetLightColor(SunsetColor);

	// Луна (всегда на противоположной стороне от солнца, но не строго 180°)
	if (MoonLight)
	{
		// Смещение на 160° вместо 180°, чтобы избежать полной темноты
		float MoonPitch = SunPitch + 160.f;
		MoonLight->SetActorRotation(FRotator(-MoonPitch, 0, 0));

		float MoonFactor = 1.f - FMath::Clamp(FMath::Sin(FMath::DegreesToRadians(SunPitch + 20.f)), 0.f, 1.f);
		ULightComponent* MoonComp = MoonLight->GetLightComponent();
		MoonComp->SetIntensity(FMath::Lerp(0.f, MoonMaxIntensity, MoonFactor));
		// Цвет задаётся из редактора
	}

	// SkyLight и Indoor
	if (USkyLightComponent* SkyComp = SkyLight->GetLightComponent())
	{
		if (SunFactor <= KINDA_SMALL_NUMBER)
		{
			SkyComp->SetIntensity(IndoorAmbientIntensity);
			SkyComp->SetLightColor(IndoorAmbientColor);
		}
		else
		{
			SkyComp->SetIntensity(FMath::Lerp(IndoorAmbientIntensity, SkyLightMaxIntensity, SunFactor));
			SkyComp->SetLightColor(SunsetColor);
		}
		SkyComp->MarkRenderStateDirty();
	}

	// Обновление SkySphere (если используется)
	if (SkySphereActor)
	{
		if (UFunction* Fn = SkySphereActor->FindFunction(FName("UpdateSunDirection")))
		{
			SkySphereActor->ProcessEvent(Fn, nullptr);
		}
	}
}

void ADayNightCycleManager::CollectAllNightLights()
{
	TArray<AActor*> All;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), All);

	for (AActor* A : All)
	{
		TArray<ULightComponent*> Lights;
		A->GetComponents(Lights);

		for (ULightComponent* L : Lights)
		{
			if (Cast<UPointLightComponent>(L) || Cast<USpotLightComponent>(L))
				ToggleableLights.Add(FManagedLight(L));
		}

		TArray<UCandleFlickerComponent*> Flicks;
		A->GetComponents(Flicks);
		CandleFlickers.Append(Flicks);

		TArray<UMaterialSwitcherComponent*> Mats;
		A->GetComponents(Mats);
		MaterialSwitchers.Append(Mats);
	}
}

void ADayNightCycleManager::UpdateNightLights(float SunFactor, float DeltaTime)
{
	float Target = (SunFactor < NightThreshold) ? 3.f : 0.f;

	for (auto& M : ToggleableLights)
		if (M.Light)
			M.Light->SetIntensity(FMath::FInterpTo(M.Light->Intensity, M.BaseIntensity * Target, DeltaTime, FadeSpeed));

	for (auto* Flick : CandleFlickers)
		if (Flick)
			Flick->SetComponentTickEnabled(Target >= 0.5f);
}

void ADayNightCycleManager::UpdateNightActors(bool bEnable)
{
	for (AActor* A : NightActiveActors)
		if (A)
		{
			A->SetActorHiddenInGame(!bEnable);
			A->SetActorEnableCollision(bEnable);
			A->SetActorTickEnabled(bEnable);
		}
}

void ADayNightCycleManager::UpdateMaterialSwitchers(bool bNight)
{
	for (auto* Sw : MaterialSwitchers)
		if (Sw)
			Sw->SetIsNight(bNight);
}
