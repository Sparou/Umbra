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

    CurrentTime = FMath::Clamp(StartTimeOfDay, 0.f, DayLengthInSeconds);
    CollectAllNightLights();
}

void ADayNightCycleManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CurrentTime += DeltaTime;
    if (CurrentTime > DayLengthInSeconds)
        CurrentTime -= DayLengthInSeconds;

    float Normalized = CurrentTime / DayLengthInSeconds;

    UpdateLighting(Normalized);

    float SunAngle  = Normalized * 360.f - 90.f;
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

    // 1) Поворот солнца
    float SunPitch = NormalizedTime * 360.f - 90.f;
    SunLight->SetActorRotation(FRotator(-SunPitch, 0, 0));

    // 2) Высота солнца
    float SunFactor = FMath::Clamp(FMath::Sin(FMath::DegreesToRadians(SunPitch)), 0.f, 1.f);

    // 3) Интенсивность солнца
    ULightComponent* SunComp = SunLight->GetLightComponent();
    SunComp->SetIntensity(FMath::Lerp(0.f, SunMaxIntensity, SunFactor));

    // 4) Цвет солнца: линейная RGB-интерполяция NightColor → DawnColor → DayColor
    FLinearColor CurrentColor;
    if (SunFactor < 0.3f)
    {
        float T = SunFactor / 0.3f;
        CurrentColor = NightColor + (DawnColor - NightColor) * T;  // тёмно-синий → оранжевый
    }
    else
    {
        float T = (SunFactor - 0.3f) / 0.7f;
        CurrentColor = DawnColor + (DayColor - DawnColor) * T;    // оранжевый → дневной
    }
    SunComp->SetLightColor(CurrentColor);

    // 5) Луна (если задана)
    if (MoonLight)
    {
        float MoonPitch = SunPitch + 160.f;
        MoonLight->SetActorRotation(FRotator(-MoonPitch, 0, 0));

        float MoonFactor = 1.f - SunFactor;
        ULightComponent* MoonComp = MoonLight->GetLightComponent();
        MoonComp->SetIntensity(FMath::Lerp(0.f, MoonMaxIntensity, MoonFactor));
    }

    // 6) SkyLight / Indoor ambient
    if (USkyLightComponent* SkyComp = SkyLight->GetLightComponent())
    {
        if (SunFactor <= KINDA_SMALL_NUMBER)
        {
            SkyComp->SetIntensity(IndoorAmbientIntensity);
            SkyComp->SetLightColor(IndoorAmbientColor);
        }
        else
        {
            float I = FMath::Lerp(IndoorAmbientIntensity, SkyLightMaxIntensity, SunFactor);
            SkyComp->SetIntensity(I);
            SkyComp->SetLightColor(CurrentColor);
        }
        SkyComp->MarkRenderStateDirty();
    }

    // 7) Обновление BP_Sky_Sphere (если используется)
    if (SkySphereActor)
    {
        if (UFunction* Fn = SkySphereActor->FindFunction(FName("UpdateSunDirection")))
            SkySphereActor->ProcessEvent(Fn, nullptr);
    }
}

void ADayNightCycleManager::CollectAllNightLights()
{
    ToggleableLights.Empty();
    CandleFlickers.Empty();
    MaterialSwitchers.Empty();

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
    float TargetMul = (SunFactor < NightThreshold) ? 3.f : 0.f;

    for (auto& M : ToggleableLights)
    {
        if (!M.Light) continue;
        float NewI = FMath::FInterpTo(M.Light->Intensity, M.BaseIntensity * TargetMul, DeltaTime, FadeSpeed);
        M.Light->SetIntensity(NewI);
    }

    for (auto* Flick : CandleFlickers)
    {
        if (Flick) Flick->SetComponentTickEnabled(TargetMul > 0.5f);
    }
}

void ADayNightCycleManager::UpdateNightActors(bool bEnable)
{
    for (AActor* A : NightActiveActors)
    {
        if (!A) continue;
        A->SetActorHiddenInGame(!bEnable);
        A->SetActorEnableCollision(bEnable);
        A->SetActorTickEnabled(bEnable);
    }
}

void ADayNightCycleManager::UpdateMaterialSwitchers(bool bNight)
{
    for (auto* Sw : MaterialSwitchers)
    {
        if (Sw) Sw->SetIsNight(bNight);
    }
}
