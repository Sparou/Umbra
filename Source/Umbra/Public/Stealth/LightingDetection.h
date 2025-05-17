// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LightingDetection.generated.h"

class USkyLightComponent;
class UDirectionalLightComponent;
class USkeletalMeshComponent;
class ULightComponent;

UENUM(BlueprintType)
enum class ELightType : uint8
{
    Point       UMETA(DisplayName = "Point Light"),
    Spot        UMETA(DisplayName = "Spot Light"),
    Sky         UMETA(DisplayName = "Sky Light"),
    Directional UMETA(DisplayName = "Directional Light")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UMBRA_API ULightingDetection : public UActorComponent
{
    GENERATED_BODY()

public:
    ULightingDetection();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    float LightPercentage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    float DetectionRadius = 800.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    float MaxLightValue = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    int32 TraceSampleCount = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    TMap<ELightType, float> LightTypeWeights;

    /** Использовать только Socket 'head' вместо всех костей */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    bool bTraceOnlyHead = true;

    /** Автоматически собирать источники при старте */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    bool bAutoGatherLights = true;

    UFUNCTION(BlueprintCallable, Category = "Lighting")
    float GetLightPercentage() const { return LightPercentage; }

    UFUNCTION(BlueprintCallable, Category = "Lighting")
    void GatherLightSources();

protected:
    virtual void OnRegister() override;
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    TArray<ULightComponent*> LightSources;
    USkeletalMeshComponent* OwnerMesh = nullptr;

    void ProcessLocalLight(ULightComponent* LightComp, const FVector& OwnerLoc, float& OutTotal);
    void ProcessSkyLight(USkyLightComponent* SkyComp, float& OutTotal);
    void ProcessDirectionalLight(UDirectionalLightComponent* DirComp, float& OutTotal);

    bool IsOccluded(const FVector& From, const FVector& To) const;
    bool IsDirectionalLightOccluded(const FVector& Direction, const FVector& TargetPos) const;
    TArray<FVector> GetTracePoints() const;
};
