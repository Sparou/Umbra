// Copyrighted by Vorona Games

#include "Character/Component/StealthComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "UmbraGameplayTags.h"
#include "Stealth/VisibilityCalculationTask.h"
#include "Subsystem/World/LightSubsystem.h"

DEFINE_LOG_CATEGORY(UmbraStealthComponentLog)

UStealthComponent::UStealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	CurrentVisibilityTask = nullptr;
}

void UStealthComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(
		VisibilityCheckTimerHandle,
		this, &UStealthComponent::TriggerVisibilityCheck,
		VisibilityUpdateInterval,
		true
		);
}

void UStealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(VisibilityCheckTimerHandle);

	if (CurrentVisibilityTask)
	{
		CurrentVisibilityTask->EnsureCompletion();
		delete CurrentVisibilityTask;
		CurrentVisibilityTask = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}


void UStealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CurrentVisibilityTask || !CurrentVisibilityTask->IsDone())
	{
		return;
	}

	if (GetOwnerASC() && VisibilityUpdateEffect)
	{
		const float CalculatedVisibility = CurrentVisibilityTask->GetTask().ResultVisibility;
		FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(VisibilityUpdateEffect, 1.f, OwnerASC->MakeEffectContext());

		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(FUmbraGameplayTags::Get().Effect_Attribute_Visibility, CalculatedVisibility);
			OwnerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	delete CurrentVisibilityTask;
	CurrentVisibilityTask = nullptr;
}

void UStealthComponent::TriggerVisibilityCheck()
{
	if (CurrentVisibilityTask)
	{
		return;
	}

	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();

	if (!Owner || !World)
	{
		return;
	}

	TArray<FVector> WorldSampleLocations;
	if (const USkeletalMeshComponent* SkeletalMesh = Owner->FindComponentByClass<USkeletalMeshComponent>())
	{
		for (const FName& SocketName : SampleSocketNames)
		{
			WorldSampleLocations.Add(SkeletalMesh->GetSocketLocation(SocketName));
		}
	}

	if (WorldSampleLocations.IsEmpty())
	{
		WorldSampleLocations.Add(GetOwner()->GetActorLocation());
	}

	ULightSubsystem* LightSubsystem = World->GetSubsystem<ULightSubsystem>();
	if (!LightSubsystem)
	{
		return;
	}

	CurrentVisibilityTask = new FAsyncTask<FVisibilityCalculationTask>(
		World,
		WorldSampleLocations,
		LightSubsystem->GetLightSourcesInRadius(GetOwner()->GetActorLocation(), LightSearchRadius),
		MinAmbientVisibility,
		FalloffCurve);
	
	CurrentVisibilityTask->StartBackgroundTask();
}

UAbilitySystemComponent* UStealthComponent::GetOwnerASC()
{
	if (OwnerASC.IsValid())
	{
		return OwnerASC.Get();
	}

	if (!GetOwner())
	{
		UE_LOG(UmbraStealthComponentLog, Error, TEXT("Owner is nullptr in [%s]"), *GetNameSafe(this));
		return nullptr;
	}

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		OwnerASC = ASC;
		return ASC;
	}

	UE_LOG(UmbraStealthComponentLog, Error, TEXT("[%s] owner does not have ASC"), *GetNameSafe(this));
	return nullptr;
}

