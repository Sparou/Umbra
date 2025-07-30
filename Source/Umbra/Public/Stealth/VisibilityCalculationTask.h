
#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"
#include "Engine/World.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

struct FVisibilityDetector;

class FVisibilityCalculationTask : public FNonAbandonableTask
{
public:

	FVisibilityCalculationTask(
		UWorld* InWorld,
		const TArray<FVisibilityDetector>& InVisibilityDetectors,
		const TArray<TWeakObjectPtr<AActor>>& InLightSources,
		float InMinAmbientVisibility,
		const TObjectPtr<UCurveFloat>& InFalloffCurve)
		: World(InWorld)
		, VisibilityDetectors(InVisibilityDetectors)
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
	TArray<FVisibilityDetector> VisibilityDetectors;
	TArray<TWeakObjectPtr<AActor>> LightSources;
	float MinAmbientVisibility;
	UCurveFloat* FalloffCurve;

	float CalculateLightContribution(const ULightComponent* LightComponent, const FVisibilityDetector& Detector);
};
