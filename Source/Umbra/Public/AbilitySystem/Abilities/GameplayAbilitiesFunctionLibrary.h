// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayAbilitiesFunctionLibrary.generated.h"

class UGameplayAbility;
struct FGameplayTag;
/**
 * 
 */
UCLASS()
class UMBRA_API UGameplayAbilitiesFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * @param Actor Актор, относительную позицию которого необходимо получить
	 * @param Target Актор, относительно которого будет вычеслена позиция
	 * @param HeightDifferenceThreshold Значение, определяющие максимальную
	 * разницу в высоте для идентефикации позиции как "Top"
	 * @return Сторона, с которой находится актор относительно цели
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities|Utilities")
	static FGameplayTag GetActorPositionRelativeToTarget(const AActor* Actor, const AActor* Target, float HeightDifferenceThreshold);

	/**
	 * @param Ability Cпособность, из которой был вызван метод.
	 * @param Actor Актор, цель которого должна быть вычислена.
	 * @param BlackboardVariableName Для ИИ - переменная Blackboard, в которой хранится текущая цель.
	 * @param BoneName Для ИИ - имя кости на цели, куда необходимо прицелиться.
	 * @return Точка, на которую нацелен актор.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities|Utilities")
	static FVector GetTargetLocation(UGameplayAbility* Ability,
		AActor* Actor,
		FName BlackboardVariableName = "CurrentEnemy",
		FName BoneName = "spine_03");

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities|Utilities")
	static FRotator GetRotationToTarget(FVector CurrentLocation, FVector TargetLocation);
	
};