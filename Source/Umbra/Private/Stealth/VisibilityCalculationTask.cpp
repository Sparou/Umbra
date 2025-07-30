// Copyrighted by Vorona Games

#include "Stealth/VisibilityCalculationTask.h"
#include "Character/Component/StealthComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/DirectionalLightComponent.h"

void FVisibilityCalculationTask::DoWork()
{
	if (VisibilityDetectors.IsEmpty())
	{
		ResultVisibility = MinAmbientVisibility;
		return;
	}

	TArray<float> PointVisibilities;
	PointVisibilities.Reserve(VisibilityDetectors.Num());

	//ParallelFor()??
	
	for (const FVisibilityDetector& Detector : VisibilityDetectors)
	{
		float TotalPointVisibility = MinAmbientVisibility;

		for (const auto& WeakLight : LightSources)
		{
			if (!WeakLight.IsValid())
			{
				continue;
			}

			AActor* LightActor = WeakLight.Get();

			FHitResult HitResult;
			const bool bTraceHit = World->LineTraceSingleByChannel(
				HitResult,
				Detector.Location,
				LightActor->GetActorLocation(),
				ECollisionChannel::ECC_Visibility
				);
			
			if (bTraceHit && HitResult.GetActor() != LightActor)
			{
				continue;
			}

			if (ULightComponent* LightComponent = LightActor->FindComponentByClass<ULightComponent>())
			{
				TotalPointVisibility += CalculateLightContribution(LightComponent, Detector);
			}
		}

		PointVisibilities.Add(FMath::Clamp(TotalPointVisibility, 0.0f, 1.0f));
	}

	if (PointVisibilities.Num() > 0)
	{
		ResultVisibility = FMath::Max(PointVisibilities);
	}
	else
	{
		ResultVisibility = MinAmbientVisibility;
	}
}

float FVisibilityCalculationTask::CalculateLightContribution(const ULightComponent* LightComponent, const FVisibilityDetector& Detector)
{
	float Contribution = 0.0f;
	float Divider = GetDefault<UPointLightComponent>()->Intensity;
	const float BaseFactor = LightComponent->Intensity / Divider;
	
	if (const USpotLightComponent* SpotLight = Cast<USpotLightComponent>(LightComponent))
	{
		const float Distance = FVector::Dist(Detector.Location, SpotLight->GetComponentLocation());
		const float AttenuationRadius = SpotLight->AttenuationRadius;

		if (Distance < AttenuationRadius)
		{
			const FVector DirectionToSample = (Detector.Location - SpotLight->GetComponentLocation()).GetSafeNormal();
			const FVector LightForward = SpotLight->GetForwardVector();

			// Вычисляем, насколько похоже направление от PC к источнику света и от источника света к свету
			const float DotProduct = FVector::DotProduct(DirectionToSample, LightForward);
			// Для оптимизации берем косинус угла света, который пороговым значением, определяющим, попали ли мы в "прожектор"
			const float CosOuterCone = FMath::Cos(FMath::DegreesToRadians(SpotLight->OuterConeAngle));

			if (DotProduct >= CosOuterCone)
			{
				float DistanceAttenuation;

				if (FalloffCurve)
				{
					const float DistanceRatio = Distance / AttenuationRadius;
					DistanceAttenuation = FalloffCurve->GetFloatValue(1 - DistanceRatio);
				}
				else
				{
					DistanceAttenuation = FMath::Pow(1.0f - Distance / AttenuationRadius, 5.0f);
				}

				const float CosInnerCone = FMath::Cos(FMath::DegreesToRadians(SpotLight->InnerConeAngle));
				const float AngleMultiplier = FMath::GetMappedRangeValueClamped(
					FVector2D(CosOuterCone, CosInnerCone), // Шкала
					FVector2D(0.f, 1.f), // Диапазон
					DotProduct // Значение относительно шкалы
				);

				Contribution = BaseFactor * DistanceAttenuation * AngleMultiplier;
				// UE_LOG(LogTemp, Log, TEXT("Attenuation = [%f]"), DistanceAttenuation);
				// UE_LOG(LogTemp, Log, TEXT("AngleMultiplier = [%f]"), AngleMultiplier);
				// UE_LOG(LogTemp, Log, TEXT("Base Factor = [%f]"), BaseFactor);
				// UE_LOG(LogTemp, Log, TEXT("Contribution = [%f]"), Contribution);
			}
		}
	}
	
	else if (const UPointLightComponent* PointLight = Cast<UPointLightComponent>(LightComponent))
	{
		const float Distance = FVector::Dist(Detector.Location, PointLight->GetComponentLocation());
		const float AttenuationRadius = PointLight->AttenuationRadius;

		if (Distance < AttenuationRadius)
		{
			float Attenuation;
			
			if (FalloffCurve)
			{
				const float DistanceRatio = Distance / AttenuationRadius;
				Attenuation = FalloffCurve->GetFloatValue(1 - DistanceRatio);
			}
			else
			{
				Attenuation = FMath::Pow(1.0f - Distance / AttenuationRadius, 5.0f);
			}
			
			Contribution = Attenuation * BaseFactor;
			// UE_LOG(LogTemp, Log, TEXT("Attenuation = [%f]"), Attenuation);
			// UE_LOG(LogTemp, Log, TEXT("Base Factor = [%f]"), BaseFactor);
			// UE_LOG(LogTemp, Log, TEXT("Contribution = [%f]"), Contribution);
		}
	}

	else if (Cast<UDirectionalLightComponent>(LightComponent))
	{
		Contribution = BaseFactor;
	}

	Contribution *= Detector.ContributionMultiplier;
	
	// UE_LOG(UmbraStealthComponentLog,
	// 	Log,
	// 	TEXT("Visibility Contribution = [%f] for [%s] Detector and [%s] light source"),
	// 	Contribution,
	// 	*Detector.SocketName.ToString(),
	// 	*GetNameSafe(LightComponent->GetOwner()))
	
	return Contribution;
}
