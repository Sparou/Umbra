#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "Abilities/GameplayAbilityTargetActor_Trace.h"
#include "GameplayAbilityTargetActor_Teleport.generated.h"


/**
 * AGameplayAbilityATagetActor_Teleport
 *
 * TargetActor, предназначенный для способностей телепортации.
 * Комплексно определяет точку:
 * - Если игрок целится в горизонтальную поверхность (пол), возвращает эту точку.
 * - Если игрок целится в стену, пытается найти подходящий уступ над точкой попадания.
 * - Дополнительно проводится проверка на наличие свободного пространства для капсулы персонажа.
 * 
 */
UCLASS()
class UMBRA_API AGameplayAbilityTargetActor_Teleport : public AGameplayAbilityTargetActor_Trace
{
	GENERATED_BODY()

protected:

	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual FHitResult PerformTrace(AActor* InSourceActor) override;

	/**
	 * Проверяет, достаточно ли места для капсулы персонажа в указанной точке.
	 * @param TargetLocation Центр предполагаемого места для телепортации.
	 * @param InSourceActor Персонаж, для которого выполняется проверка.
	 * @return true, если место свободно.
	 */
	bool HasEnoughSpace(const FVector& TargetLocation, AActor* InSourceActor) const;

	/**
	 * Выполняет основную трассировку от камеры игрока.
	 * @param OutHitResult Результат трассировки.
	 * @param InSourceActor Актер, от имени которого выполняется трассировка.
	 */
	void PerformPrimaryTrace(FHitResult& OutHitResult, AActor* InSourceActor) const;

	/** Количество итераций цикла поиска горизонатльной поверхности, в случае попадания в стену. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = Targeting)
	int32 WallHorizontalOffsetIterations;

	/** Шаг дистанции, на которую будет смещаться VerticalTraceStart при поиске горизонтальной поверхности, в случае попадания в стену. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = Targeting)
	float WallHorizontalOffsetStep;

	/** Максимальная высота стена, вычисляемое от точки попадания. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = Targeting)
	float MaxWallHeight;

	void ShowReticle(const FVector& ReticleLocation);
	void HideReticle();
};
