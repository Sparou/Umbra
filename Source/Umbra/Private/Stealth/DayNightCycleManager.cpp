#include "Stealth/DayNightCycleManager.h"

#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/OutputDeviceNull.h"
#include "UObject/ConstructorHelpers.h"

ADayNightCycleManager::ADayNightCycleManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADayNightCycleManager::BeginPlay()
{
	Super::BeginPlay();
}

void ADayNightCycleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentTime += DeltaTime;
	float NormalizedTime = FMath::Fmod(CurrentTime, DayLengthInSeconds) / DayLengthInSeconds;
	UpdateLighting(NormalizedTime);
}

void ADayNightCycleManager::UpdateLighting(float NormalizedTime)
{
	if (!SunLight || !SkyLight || !SkySphereActor) return;

	// Инвертированный угол (ночь = -90, день = +90)
	float PitchAngle = NormalizedTime * 360.f - 90.f;
	FRotator NewRotation = FRotator(-PitchAngle, 0.f, 0.f); // ИНВЕРТИРОВАННЫЙ угол
	SunLight->GetLightComponent()->SetWorldRotation(NewRotation);

	// Интенсивность по синусу (0 ночью, 1 днем)
	float SunIntensity = FMath::Clamp(FMath::Sin(FMath::DegreesToRadians(PitchAngle)), 0.f, 1.f);
	SunLight->GetLightComponent()->SetIntensity(SunIntensity * 10.0f);

	// SkyLight тоже адаптируется
	if (USkyLightComponent* SkyComp = SkyLight->GetLightComponent())
	{
		SkyComp->SetIntensity(SunIntensity * 3.0f);
		SkyComp->MarkRenderStateDirty();
	}

	// Обновляем SkySphere через Blueprint функцию
	UFunction* UpdateFunc = SkySphereActor->FindFunction(FName("UpdateSunDirection"));
	if (UpdateFunc)
	{
		SkySphereActor->ProcessEvent(UpdateFunc, nullptr);
	}
}


