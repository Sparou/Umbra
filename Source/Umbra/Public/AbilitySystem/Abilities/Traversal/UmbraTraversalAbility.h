// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"
#include "CollisionQueryParams.h"
#include "Character/Data/TraversalActionsData.h"
#include "UmbraTraversalAbility.generated.h"

class UTraversalActionsData;
class AUmbraBaseCharacter;
class UMotionWarpingComponent;
class UCharacterMovementComponent;
class UCapsuleComponent;

USTRUCT(BLueprintType)
struct FTraversalActionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FHitResult ObstacleHitResult;
	UPROPERTY(BlueprintReadOnly)
	FVector LedgePoint;
	UPROPERTY(BlueprintReadOnly)
	FVector VaultPoint;
	UPROPERTY(BlueprintReadOnly)
	float Height;
	UPROPERTY(BlueprintReadOnly)
	float Depth;
};

UCLASS()
class UMBRA_API UUmbraTraversalAbility : public UUmbraBaseGameplayAbility
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Acceccors")
	AUmbraBaseCharacter* GetUmbraCharacter() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Acceccors")
	UCharacterMovementComponent* GetCharacterMovementComponent() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Acceccors")
	UCapsuleComponent* GetCapsuleComponent() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Acceccors")
	UMotionWarpingComponent* GetMotionWarpingComponent() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Acceccors")
	UTraversalActionsData* GetTraversalActions() const;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag TraversalActionTag;
	UPROPERTY(BlueprintReadWrite)
	FTraversalActionMontage TraversalActionMontage;
	
	/**
	 * Поиск препятствия с помощью Box Overlap.
	 * @param ObstacleTag Gameplay Tag, в который сохранится информация о найденном препятствии.
	 * @param DetectionDistance Максимальное расстояние, на котором возможно обнаружение препятствия.
	 * @param BoxHalfSize Половина размера куба, с помощью которого будет определятся наличие препятствия.
	 * @param bDrawDebug Режим отладки.
	 * @return True, если препятствие найдено, иначе - false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Obstacle|Detection")
	bool DetectObstacle(FGameplayTag& ObstacleTag, float DetectionDistance, const FVector& BoxHalfSize = FVector(50, 30, 70), bool bDrawDebug = false);

	/**
	 * Поиск препятствия с цикла линейных трассировок.
	 * @param ObstacleHitResult HitResult, в который сохранится информация об Obstacle Hit.
	 * @param InitialDownOffset Величина изначального смещения относительно Avatar Location.
	 * @param Iterations Количество итераций цикла, смещающего начало трейса в случае отсутствия попадания.
	 * @param DetectionDistance Максимальную дистанция, на которой может быть найдено препятствие. 
	 * @param OffsetStep Величина, на которую будет смещаться трейс в случае отсутствия попадания.
	 * @param bDrawDebug Режим отладки.
	 * @return True, если препятствие было найдено, иначе - false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Obstacle|Detection")
	bool FindObstacleHitResult(FHitResult& ObstacleHitResult, float InitialDownOffset, const int32 Iterations, float DetectionDistance, float OffsetStep,  bool bDrawDebug = false);

	/**
	 * Поиск точки края препятствия с помощью цикла линейных трассировок.
	 * @param EdgeResult HitResult, в который сохранится информация об Obstacle Edge.
	 * @param ObstacleHitResult HitResult, содержащий информацию об точке попадания в Obstacle.
	 * @param Iterations Количество итераций цикла, смещающего начало трейса в случае отсутствия попадания.
	 * @param OffsetStep Величина, на которую будет смещаться трейс в случае отсутствия попадания.
	 * @param DetectionDistance Максимальную дистанция, на которой может быть найдено препятствие. 
	 * @param Threshold Величина, характеризующая минимальный порог разницы дистанций точек попадания для определения края препятствия.
	 * @param bDrawDebug Режим отладки.
	 * @return True, если край вертикальной поверхности препятствия был найден, иначе - false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Obstacle|Detection")
	bool FindObstacleEdgeResult(FHitResult& EdgeResult, const FHitResult& ObstacleHitResult, int32 Iterations, float OffsetStep, float DetectionDistance, float Threshold, bool bDrawDebug = false);

	/**
	 * Поиск точки на горизонтальной плоскости препятствия с помощью цикла линейных трассировок.
	 * @param TopHitResult HitResult, в который сохранится информация об Obstacle Top.
	 * @param EdgeHitResult HitResult, содержащий информацию об Obstacle Edge.
	 * @param Iterations Количество итераций цикла, смещающего начало трейса в случае отсутствия попадания.
	 * @param TraceHeight Высота, на которую будет поднята начальная точка трейса. Рассчитывается от Obstacle Edge.
	 * @param OffsetStep Величина, на которую будет смещаться трейс в случае отсутствия попадания.
	 * @param bDrawDebug Режим отладки.
	 * @return True, если горизонтальная поверхность препятствия была найдена, иначе - false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Obstacle|Detection")
	bool FindObstacleTopResult(FHitResult& TopHitResult, const FHitResult& EdgeHitResult, int32 Iterations, float TraceHeight, float OffsetStep, bool bDrawDebug = false);

	/**
	 * Поиск глубины препятствия с помощью цикла трассировок.
	 * @param DepthHitResult HitResult, в который сохранится информация о глубине препятствия.
	 * @param TopHitResult HitResult, содержащий информацию о ранее найденной точке горизонтальной поверхности препятствия.
	 * @param Direction Направление по которому будет определяться глубина препятствия.
	 * @param Iterations Количество итераций цикла, смещающего начало трейса в случае отсутствия попадания.
	 * @param OffsetStep Величина, на которую будет смещаться трейс в случае отсутствия попадания.
	 * @param Threshold Величина, характеризующая допустимое колебание высоты горизонтальной поверхности препятствия.
	 * @param bDrawDebug Режим отладки.
	 * @return True, если глубина препятствия и его конец были найдены.
	 */
	UFUNCTION(BlueprintCallable, Category = "Obstacle|Detection")
	bool FindObstacleDepthResult(FHitResult& DepthHitResult, const FHitResult& TopHitResult, const FVector& Direction, int32 Iterations, float OffsetStep, float Threshold, bool bDrawDebug = false);

	/**
	 * Поиск Vault Point с помощью направленного вертикально вниз трейса из смещенной относительно Depth Result точки.
	 * @param VaultResult HitResult, в который сохранится информация о Vault Point.
	 * @param DepthResult HitResult, содержащий информацию о ранее найденной точке глубины препятствия.
	 * @param Distance Расстояние от DepthResult на котором будет производиться поиск VaultPoint.
	 * @param MaxHeight Максимально допустимая высота препятствия.
	 * @param bDrawDebug Режим отладки.
	 * @return True, если Vault Point найдена, иначе - false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Obstacle|Detection")
	bool FindVaultResult(FHitResult& VaultResult, const FHitResult& DepthResult, float Distance, float MaxHeight, bool bDrawDebug);
	
	/**
	 * Проверка, есть ли место для размещения в указанной точке в пространстве.
	 * @param CapsuleHalfHeight Половина высоты капсулы персонажа.
	 * @param CapsuleHalfRadius Половина радиуса капсулы персонажа.
	 * @param OverlapLocation Точка в пространстве для проверки.
	 * @param bDrawDebug Режим отладки.
	 * @return True, если место свободно, иначе - false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Obstacle|Detection")
	bool HasEnoughSpace(float CapsuleHalfHeight, float CapsuleHalfRadius, const FVector& OverlapLocation, const TArray<AActor*>& ActorsToIgnore, bool bDrawDebug);
	
	UFUNCTION(BlueprintCallable, Category = "MotionWarping")
	void FindWarpLocation(FVector& WarpLocation, const FVector& InitialLocation, const FRotator& Rotation, float XOffset, float YOffset, float ZOffset, bool bDrawDebug);

	UFUNCTION(BlueprintCallable, Category = "MotionWarping")
	void SetupMotionWarping(const FVector& TopImpactPoint, const FVector& DepthImpactPoint, const FVector& VaultImpactPoint, const FRotator& TargetRotation, bool bDrawDebug);
	
	UFUNCTION(BlueprintCallable, Category = "Utility")
	void ApplyTraversalSettings();

	UFUNCTION(BlueprintCallable, Category = "Utility")
	void RestoreTraversalSettings();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility")
	void ReverseNormal(FRotator& ReversedNormal, const FVector& Normal);
	
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

private:

	FVector MoveVector(const FVector& Source, float Offset, const FGameplayTag& DirectionTag, const FRotator& Rotator = FRotator::ZeroRotator) const;
	
	TWeakObjectPtr<AUmbraBaseCharacter> UmbraCharacter;
	TWeakObjectPtr<UCharacterMovementComponent> Movement;
	TWeakObjectPtr<UCapsuleComponent> Capsule;
	TWeakObjectPtr<UMotionWarpingComponent> MotionWarping;
	TWeakObjectPtr<UTraversalActionsData> TraversalActions;
};
