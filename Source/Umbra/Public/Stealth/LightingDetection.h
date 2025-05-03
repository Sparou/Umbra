// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "LightingDetection.generated.h"

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

    /** Текущий уровень освещённости в процентах (0–100%) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    float LightPercentage;

    /** Радиус, в пределах которого ищем локальные источники света */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    float DetectionRadius;

    /** Максимальное значение света для нормализации */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    float MaxLightValue;

    /** Количество костей/точек, по которым трассируется свет */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    int32 TraceSampleCount;

    /** Веса влияния каждого типа источника света */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    TMap<ELightType, float> LightTypeWeights;

    /** Возвращает текущий уровень освещённости в процентах (0–100) */
    UFUNCTION(BlueprintCallable, Category = "Lighting")
    float GetLightPercentage() const { return LightPercentage; }

protected:
    virtual void OnRegister() override;
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    /** Список активных световых компонентов на сцене */
    TArray<ULightComponent*> LightSources;

    /** Скелетный меш владельца (для трассировки по костям) */
    USkeletalMeshComponent* OwnerMesh;

    /** Сканирование сцены на предмет источников света */
    void GatherLightSources();

    /** Обработка PointLight и SpotLight */
    void ProcessLocalLight(ULightComponent* LightComp, const FVector& OwnerLocation, float& OutTotal);

    /** Обработка SkyLight */
    void ProcessSkyLight(USkyLightComponent* SkyComp, float& OutTotal);

    /** Обработка DirectionalLight с учётом теней */
    void ProcessDirectionalLight(UDirectionalLightComponent* DirComp, float& OutTotal);

    /** Проверка: перекрыт ли луч от источника до цели */
    bool IsOccluded(const FVector& LightPos, const FVector& TargetPos) const;

    /** Проверка: перекрыт ли луч от неба до объекта при DirectionalLight */
    bool IsDirectionalLightOccluded(const FVector& Direction, const FVector& TargetPos) const;
};
