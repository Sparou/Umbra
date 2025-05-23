#include "Stealth/LightingDetection.h"
#include "EngineUtils.h"
#include "Engine/Light.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Stealth/CandleFlickerComponent.h"
#include "DrawDebugHelpers.h"

ULightingDetection::ULightingDetection()
{
    PrimaryComponentTick.bCanEverTick = true;

    LightTypeWeights.Add(ELightType::Point, 1.f);
    LightTypeWeights.Add(ELightType::Spot, 1.f);
    LightTypeWeights.Add(ELightType::Sky, 1.f);
    LightTypeWeights.Add(ELightType::Directional, 0.5f);
}

void ULightingDetection::OnRegister()
{
    Super::OnRegister();
    if (bAutoGatherLights)
    {
        GatherLightSources();
    }
}

void ULightingDetection::BeginPlay()
{
    Super::BeginPlay();
    OwnerMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
}

void ULightingDetection::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!OwnerMesh) return;

    const FVector OwnerLoc = OwnerMesh->GetComponentLocation();
    float TotalLight = 0.f;

    for (ULightComponent* Comp : LightSources)
    {
        if (!Comp || !Comp->IsVisible()) continue;

        if (auto* Sky = Cast<USkyLightComponent>(Comp))
            ProcessSkyLight(Sky, TotalLight);
        else if (auto* Dir = Cast<UDirectionalLightComponent>(Comp))
            ProcessDirectionalLight(Dir, TotalLight);
        else
            ProcessLocalLight(Comp, OwnerLoc, TotalLight);
    }

    LightPercentage = FMath::Clamp(TotalLight / MaxLightValue, 0.f, 1.f) * 100.f;
}

void ULightingDetection::GatherLightSources()
{
    LightSources.Empty();
    UWorld* World = GetWorld();
    if (!World) return;

    for (TActorIterator<ALight> It(World); It; ++It)
    {
        if (ULightComponent* Comp = It->FindComponentByClass<ULightComponent>())
        {
            LightSources.Add(Comp);
        }
    }

    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* Actor = *It;
        if (Actor->FindComponentByClass<UCandleFlickerComponent>())
        {
            if (ULightComponent* Light = Actor->FindComponentByClass<ULightComponent>())
            {
                LightSources.AddUnique(Light);
            }
        }
    }
}

TArray<FVector> ULightingDetection::GetTracePoints() const
{
    TArray<FVector> Points;

    if (!OwnerMesh) return Points;

    if (bTraceOnlyHead && OwnerMesh->DoesSocketExist("head"))
    {
        Points.Add(OwnerMesh->GetSocketLocation("head"));
    }
    else
    {
        TArray<FName> Sockets = OwnerMesh->GetAllSocketNames();
        int32 Count = FMath::Clamp(TraceSampleCount, 1, Sockets.Num());

        for (int32 i = 0; i < Count; ++i)
        {
            Points.Add(OwnerMesh->GetSocketLocation(Sockets[i]));
        }
    }

    return Points;
}

void ULightingDetection::ProcessLocalLight(ULightComponent* LightComp, const FVector& OwnerLoc, float& OutTotal)
{
    float AttenuationRadius = 0.f;
    ELightType Type;

    if (auto* Point = Cast<UPointLightComponent>(LightComp))
    {
        AttenuationRadius = Point->AttenuationRadius;
        Type = ELightType::Point;
    }
    else if (auto* Spot = Cast<USpotLightComponent>(LightComp))
    {
        AttenuationRadius = Spot->AttenuationRadius;
        Type = ELightType::Spot;
    }
    else
    {
        return;
    }

    float Weight = LightTypeWeights.Contains(Type) ? LightTypeWeights[Type] : 1.f;
    if (Weight <= 0.f) return;

    const FVector LightPos = LightComp->GetComponentLocation();
    float Distance = FVector::Dist(OwnerLoc, LightPos);

    if (Distance > AttenuationRadius) return;

    const auto TracePoints = GetTracePoints();
    float Sum = 0.f;

    for (const FVector& Target : TracePoints)
    {
        if (!IsOccluded(LightPos, Target))
        {
            float Norm = FMath::Clamp(LightComp->Intensity / MaxLightValue, 0.f, 1.f);
            float Att = 1.f - (Distance / AttenuationRadius);
            Sum += Norm * Att * Weight;
        }
    }

    OutTotal += Sum / TracePoints.Num();
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
    if (Weight <= 0.f) return;

    const FVector Direction = -DirComp->GetComponentRotation().Vector();
    const auto TracePoints = GetTracePoints();
    float Sum = 0.f;

    for (const FVector& Target : TracePoints)
    {
        if (!IsDirectionalLightOccluded(Direction, Target))
        {
            float Norm = FMath::Clamp(DirComp->Intensity / MaxLightValue, 0.f, 1.f);
            Sum += Norm * Weight;
        }
    }

    OutTotal += Sum / TracePoints.Num();
}

bool ULightingDetection::IsOccluded(const FVector& From, const FVector& To) const
{
    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(LightTrace), true, GetOwner());
    return GetWorld()->LineTraceSingleByChannel(Hit, From, To, ECC_Visibility, Params) && Hit.GetActor() != GetOwner();
}

bool ULightingDetection::IsDirectionalLightOccluded(const FVector& Direction, const FVector& TargetPos) const
{
    FVector Start = TargetPos + Direction * 10000.f;
    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(DirectionalTrace), true, GetOwner());
    return GetWorld()->LineTraceSingleByChannel(Hit, Start, TargetPos, ECC_Visibility, Params) && Hit.GetActor() != GetOwner();
}
