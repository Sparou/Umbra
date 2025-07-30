// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "TraversalActionsData.generated.h"

struct FGameplayAbilityActivationInfo;

USTRUCT(BlueprintType)
struct FTraversalActionMontage
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage;
	
	/** Количество параметров MotionWarping, которые использует этот анимационный монтаж. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "4"))
	int32 MotionWarpingParameters = 0;
	
	/** Параметр MotionWarping. Определяет смещение для запрыгивания на препятствие. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "MotionWarpingParameters >= 1", EditConditionHides))
	FVector TopWarpOffset = FVector(0, 0, 0);

	/** Параметр MotionWarping. Определяет смещение для балансировки после запрыгивания на препятствие. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "MotionWarpingParameters >= 2", EditConditionHides))
	FVector BalanceWarpOffset = FVector(0, 0, 0);

	/** Параметр MotionWarping. Определяет смещение для преодоления препятствия. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "MotionWarpingParameters >= 3", EditConditionHides))
	FVector DepthWarpOffset = FVector(0, 0, 0);

	/** Параметр MotionWarping. Определяет смещение для точки приземления после преодоления препятствия. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "MotionWarpingParameters >= 4", EditConditionHides))
	FVector VaultWarpOffset = FVector(0, 0, 0);
	
};


USTRUCT(BlueprintType)
struct FTraversalAction
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTraversalActionMontage> Montages;

	/**
	 * Диапазон высоты для выполнения действия.
	 * X - минимальная высота, Y - максимальная высота.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D ObstacleHeightRange;

	/**
	 * Диапазон глубины для выполнения действия.
	 * X - минимальная глубина, Y - максимальная глубина.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D ObstacleDepthRange;

	/**
	 * Диапазон высоты точки преодоления препятствия.
	 * X - минимальная высота, Y - максимальная высота.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D VaultHeighRange;

	/**
	 * Диапазон скорости, необходимый персонажу, для выполнения действия.
	 * X - минимальная скорость, Y - максимальная скорость.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D SpeedRange;

	/** Тип коллизии для капсулы персонажа во время выполнения действия. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECollisionEnabled::Type> CollisionType;

	/** Режим перемещения для персонажа во время выполнения действия. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EMovementMode> MovementMode;;
};

/**
 * Data Asset, содержащий информацию о Traversal Actions, необходимых для Traversal Ability.
 */
UCLASS()
class UMBRA_API UTraversalActionsData : public UDataAsset
{

	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FTraversalAction> ActionsMap;

	/**
	 * Поиск Traversal Action, соответствующих тэгу.
	 * @param OutTraversalAction Traversal Action, в который сохранится информация о найденном значении.
	 * @param ActionTypeTag Gameplay Tag, соответсвующий типу Traversal Action.
	 * @return True, если соответствующий ActionTypeTag Traversal Action был найден и имеет хотя бы один Traversal Action Montage.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool FindActionByTag(FTraversalAction& OutTraversalAction, const FGameplayTag& ActionTypeTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool FindActionsByTag(TMap<FGameplayTag, FTraversalAction>& OutSuitableActionsMap, const FGameplayTag& ActionTypeTag) const;
	
	/**
	 * Получение случайного Traversal Action Montage из Gameplay Ability.
	 * @param OutTraversalAction Traversal Action Montage, в который сохранится информация о выборе.
	 * @param ActionTypeTag Gameplay Tag, соответсвующий типу Traversal Action.
	 * @param AbilityActivationInfo Информация о способности, в которой происходит вызов метода.
	 * @param SeedMultiplier Множитель для PredictionKey, на основе которого будет создан Random Stream.
	 * @return True, если Traversal Action Montage найден и валиден, иначе - False.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetRandomTraversalActionFromAbility(FTraversalActionMontage& OutTraversalAction, const FGameplayTag& ActionTypeTag, const FGameplayAbilityActivationInfo& AbilityActivationInfo, float SeedMultiplier = 300.f);

};
