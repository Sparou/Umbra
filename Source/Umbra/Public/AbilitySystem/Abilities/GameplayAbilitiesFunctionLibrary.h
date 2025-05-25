// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayAbilitiesFunctionLibrary.generated.h"

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
};
