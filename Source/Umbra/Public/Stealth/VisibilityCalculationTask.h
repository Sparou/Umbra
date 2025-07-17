
#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"
#include "Engine/World.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

class FVisibilityCalculationTask : public FNonAbandonableTask
{
public:

	FVisibilityCalculationTask(
		UWorld* InWorld,
		const TArray<FVector>& InSampleLocations,
		const TArray<TWeakObjectPtr<AActor>>& InLightSources,
		float InMinAmbientVisibility,
		const TObjectPtr<UCurveFloat>& InFalloffCurve)
		: World(InWorld)
		, SampleLocations(InSampleLocations)
		, LightSources(InLightSources)
		, MinAmbientVisibility(InMinAmbientVisibility)
		, FalloffCurve(InFalloffCurve) {}

	void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FVisibilityCalculationTask, STATGROUP_ThreadPoolAsyncTasks);
	}
	
	float ResultVisibility = 0.0f;

private:
	UWorld* World;
	TArray<FVector> SampleLocations;
	TArray<TWeakObjectPtr<AActor>> LightSources;
	float MinAmbientVisibility;
	UCurveFloat* FalloffCurve;

	float CalculateLightContribution(const ULightComponent* LightComponent, const FVector& SampleLocation);
};