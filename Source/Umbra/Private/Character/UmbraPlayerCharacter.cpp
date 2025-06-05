	// Copyrighted by Vorona Games


#include "Character/UmbraPlayerCharacter.h"
#include "Character/Component/InteractionComponent.h"
#include "Character/Component/TraversalComponent.h"
#include "AbilitySystem/UmbraAttributeSet.h"
#include "Stealth/LightingDetection.h"
#include "Blueprint/UserWidget.h"
#include "Stealth/LightLevelIndicator.h"
#include "Umbra/Umbra.h"

AUmbraPlayerCharacter::AUmbraPlayerCharacter(const FObjectInitializer& ObjInit)
{
	AttributeSet = CreateDefaultSubobject<UUmbraAttributeSet>("Attribute Set");
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>("Interaction Component");
	TraversalComponent = CreateDefaultSubobject<UTraversalComponent>("Traversal Component");
	LightingDetector = CreateDefaultSubobject<ULightingDetection>("Lighting Detector");
}

UAssassinationsData* AUmbraPlayerCharacter::GetAssassinationsData()
{
	return AssassinationsData;
}

const ULightingDetection* AUmbraPlayerCharacter::GetLightingDetector() const
{
return LightingDetector;
}

UAISense_Sight::EVisibilityResult AUmbraPlayerCharacter::CanBeSeenFrom(const FCanBeSeenFromContext& Context,
	FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested,
	float& OutSightStrength, int32* UserData, const FOnPendingVisibilityQueryProcessedDelegate* Delegate)
{
	if(!ThresholdOfVisibilityFromDistanceSquaredTable) return UAISense_Sight::EVisibilityResult::NotVisible;
	const FRealCurve* VisibilityFromDistanceSquaredCurve = ThresholdOfVisibilityFromDistanceSquaredTable->FindCurve(FName("VisibilityFromDistanceSquared"), TEXT("VisibilityFromDistanceTable"));
	if(!VisibilityFromDistanceSquaredCurve) return UAISense_Sight::EVisibilityResult::NotVisible;

	const float Distance = (Context.ObserverLocation - GetActorLocation()).SizeSquared();
	const float LightPercentageThreshold = VisibilityFromDistanceSquaredCurve->Eval(Distance);
	
	//UE_LOG(LogTemp, Warning, TEXT("Threshold = %f; CurLight = %f"), LightPercentageThreshold, LightingDetector->LightPercentage);
	UE_LOG(LogTemp, Warning, TEXT("CurEnemy %s has invisibility tag: %hs"), *this->GetName(),
		GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(InvisibilityTagName)) ? "true" : "false");
	
	if(LightingDetector->LightPercentage <= LightPercentageThreshold ||
		GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(InvisibilityTagName)))
	{
		return UAISense_Sight::EVisibilityResult::NotVisible;
	}
	
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Context.IgnoreActor);
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Context.ObserverLocation,
		this->GetActorLocation(),
		ECC_Visibility,
		CollisionParams
	);

	if (bHit)
	{
		DrawDebugLine(GetWorld(),
			Context.ObserverLocation,
			HitResult.ImpactPoint,
			FColor::Green,
			false,
			5.f,
			0,
			1.0f);
	}

	AActor* HitActor = Cast<AActor>(HitResult.GetActor());

	if (!bHit || (IsValid(HitActor) && HitActor->IsOwnedBy(this)))
	{
		OutSeenLocation = bHit ? HitResult.ImpactPoint : OutSeenLocation;
		OutNumberOfLoSChecksPerformed = 1;
		OutNumberOfAsyncLosCheckRequested = 0;
		OutSightStrength = 1;
		//UE_LOG(LogTemp, Warning, TEXT("%s is visible"), *this->GetName());
		return UAISense_Sight::EVisibilityResult::Visible;
	}

	OutNumberOfLoSChecksPerformed = 1;
	OutNumberOfAsyncLosCheckRequested = 0;
	OutSightStrength = 0;
	//UE_LOG(LogTemp, Warning, TEXT("%s is invisible"), *this->GetName());
	return UAISense_Sight::EVisibilityResult::NotVisible;
}

	void AUmbraPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

		
	GetMesh()->SetRenderCustomDepth(true);	
	if (IsLocallyControlled())
	{
		GetMesh()->SetCustomDepthStencilValue(77);
	}
	else
	{
		GetMesh()->SetCustomDepthStencilValue(XRAY_STENCIL_VALUE);
	}
	if (LightWidgetClass)
	{
		// Создаём и показываем наш C++-виджет
		ULightLevelIndicator* Widget = CreateWidget<ULightLevelIndicator>(GetWorld(), LightWidgetClass);
		if (Widget)
		{
			Widget->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create LightLevelIndicator widget"));
		}
	}
}

void AUmbraPlayerCharacter::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();
}
