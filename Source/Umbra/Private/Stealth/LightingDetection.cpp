// -------------------- Includes --------------------
#include "Stealth/LightingDetection.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "Engine/SkyLight.h"
#include "Engine/DirectionalLight.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Stealth/CandleFlickerComponent.h"

// -------------------- Constructor --------------------
ULightingDetection::ULightingDetection()
{
    PrimaryComponentTick.bCanEverTick = true;
    LightPercentage = 0.f;
    DetectionRadius = 800.f;
    MaxLightValue = 1.f;
    TraceSampleCount = 25;

    LightTypeWeights.Add(ELightType::Point, 1.f);
    LightTypeWeights.Add(ELightType::Spot, 1.f);
    LightTypeWeights.Add(ELightType::Sky, 1.f);
    LightTypeWeights.Add(ELightType::Directional, 1.f);
}

// -------------------- Lifecycle --------------------
void ULightingDetection::OnRegister()
{
    Super::OnRegister();
    GatherLightSources();
}

void ULightingDetection::BeginPlay()
{
    Super::BeginPlay();
    OwnerMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
}

void ULightingDetection::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* Owner = GetOwner();
    if (!Owner) return;
    FVector OwnerLoc = Owner->GetActorLocation();

    float TotalLight = 0.f;
    for (ULightComponent* Comp : LightSources)
    {
        if (!Comp) continue;
        if (auto P = Cast<UPointLightComponent>(Comp))
            ProcessLocalLight(P, OwnerLoc, TotalLight);
        else if (auto S = Cast<USpotLightComponent>(Comp))
            ProcessLocalLight(S, OwnerLoc, TotalLight);
        else if (auto Sky = Cast<USkyLightComponent>(Comp))
            ProcessSkyLight(Sky, TotalLight);
        else if (auto Dir = Cast<UDirectionalLightComponent>(Comp))
            ProcessDirectionalLight(Dir, TotalLight);
    }

    LightPercentage = FMath::Clamp(TotalLight / MaxLightValue, 0.f, 1.f) * 100.f;
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow,
            FString::Printf(TEXT("Light Level: %.1f%%"), LightPercentage));
    }
}

// -------------------- Light Gathering --------------------
void ULightingDetection::GatherLightSources()
{
    LightSources.Empty();
    UWorld* World = GetWorld();

    for (TActorIterator<APointLight> It(World); It; ++It)
    {
        if (auto Comp = It->FindComponentByClass<UPointLightComponent>())
            LightSources.Add(Cast<ULightComponent>(Comp));
    }
    for (TActorIterator<ASpotLight> It(World); It; ++It)
    {
        if (auto Comp = It->FindComponentByClass<USpotLightComponent>())
            LightSources.Add(Cast<ULightComponent>(Comp));
    }
    for (TActorIterator<ASkyLight> It(World); It; ++It)
    {
        if (auto Comp = It->FindComponentByClass<USkyLightComponent>())
            LightSources.Add(Cast<ULightComponent>(Comp));
    }
    for (TActorIterator<ADirectionalLight> It(World); It; ++It)
    {
        if (auto Comp = It->FindComponentByClass<UDirectionalLightComponent>())
            LightSources.Add(Cast<ULightComponent>(Comp));
    }

    // === Добавим свет от актёров с компонентом CandleFlickerComponent ===
    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* Actor = *It;
        if (Actor->FindComponentByClass<UCandleFlickerComponent>())
        {
            if (auto Point = Actor->FindComponentByClass<UPointLightComponent>())
            {
                LightSources.Add(Cast<ULightComponent>(Point));
            }
            else if (auto Spot = Actor->FindComponentByClass<USpotLightComponent>())
            {
                LightSources.Add(Cast<ULightComponent>(Spot));
            }
        }
    }
}

// -------------------- Processing Light --------------------
void ULightingDetection::ProcessLocalLight(ULightComponent* LightComp, const FVector& OwnerLoc, float& OutTotal)
{
    ELightType Type = LightComp->IsA<USpotLightComponent>() ? ELightType::Spot : ELightType::Point;
    float Weight = LightTypeWeights.Contains(Type) ? LightTypeWeights[Type] : 1.f;
    if (Weight <= 0.f) return;

    FVector LightPos = LightComp->GetComponentLocation();
    float Dist = FVector::Dist(OwnerLoc, LightPos);

    float Radius = DetectionRadius;
    if (auto P = Cast<UPointLightComponent>(LightComp)) Radius = P->AttenuationRadius;
    else if (auto S = Cast<USpotLightComponent>(LightComp)) Radius = S->AttenuationRadius;
    if (Dist > Radius || !OwnerMesh) return;

    auto Bones = OwnerMesh->GetAllSocketNames();
    int32 Samples = FMath::Clamp(TraceSampleCount, 1, Bones.Num());
    float Sum = 0.f;
    for (int32 i = 0; i < Samples; ++i)
    {
        FVector Target = OwnerMesh->GetSocketLocation(Bones[i]);
        if (!IsOccluded(LightPos, Target))
        {
            float Norm = FMath::Clamp(LightComp->Intensity / MaxLightValue, 0.f, 1.f);
            float Att = 1.f - (Dist / Radius);
            Sum += Norm * Att * Weight;
        }
    }
    OutTotal += Sum / Samples;
}

void ULightingDetection::ProcessSkyLight(USkyLightComponent* SkyComp, float& OutTotal)
{
    float Weight = LightTypeWeights.Contains(ELightType::Sky) ? LightTypeWeights[ELightType::Sky] : 1.f;
    if (Weight <= 0.f) return;

    float Norm = FMath::Clamp(SkyComp->Intensity / MaxLightValue, 0.f, 1.f);
    OutTotal += Norm * Weight;
}

void ULightingDetection::ProcessDirectionalLight(UDirectionalLightComponent* DirComp, float& OutTotal)
{
    float Weight = LightTypeWeights.Contains(ELightType::Directional) ? LightTypeWeights[ELightType::Directional] : 1.f;
    if (Weight <= 0.f || !OwnerMesh) return;

    auto Bones = OwnerMesh->GetAllSocketNames();
    int32 Samples = FMath::Clamp(TraceSampleCount, 1, Bones.Num());
    float Sum = 0.f;

    FVector Direction = -DirComp->GetComponentRotation().Vector();
    for (int32 i = 0; i < Samples; ++i)
    {
        FVector Target = OwnerMesh->GetSocketLocation(Bones[i]);
        if (!IsDirectionalLightOccluded(Direction, Target))
        {
            float Norm = FMath::Clamp(DirComp->Intensity / MaxLightValue, 0.f, 1.f);
            Sum += Norm * Weight;
        }
    }
    OutTotal += Sum / Samples;
}

// -------------------- Occlusion Checks --------------------
bool ULightingDetection::IsOccluded(const FVector& LightPos, const FVector& TargetPos) const
{
    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(LightTrace), true, GetOwner());
    if (GetWorld()->LineTraceSingleByChannel(Hit, LightPos, TargetPos, ECC_Visibility, Params))
        return Hit.GetActor() != GetOwner();
    return false;
}

bool ULightingDetection::IsDirectionalLightOccluded(const FVector& Direction, const FVector& TargetPos) const
{
    FVector TraceStart = TargetPos + Direction * 10000.f;
    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(DirectionalLightTrace), true, GetOwner());
    if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TargetPos, ECC_Visibility, Params))
        return Hit.GetActor() != GetOwner();
    return false;
}
