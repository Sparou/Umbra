// Copyrighted by Vorona Games


#include "AbilitySystem/Interaction/Tasks/AbilityTask_GrantNearbyInteraction.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Interaction/InteractionStatics.h"
#include "Interface/InteractionInterface.h"
#include "UmbraCollisionChannels.h"
#include "AbilitySystem/Interaction/InteractionQuery.h"
#include "Actor/UmbraInteractableActor.h"
#include "Engine/OverlapResult.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"

static int32 GCVarDrawDebugAbilityInteractionSphere = 0;
static FAutoConsoleVariableRef CVarDrawDebugInteractionSphereRef(
	TEXT("Umbra.DrawDebug.Ability.Interaction.Sphere"),
	GCVarDrawDebugAbilityInteractionSphere,
	TEXT("[0] - Disable Interaction Sphere visualization | [1] = Enable Interaction Sphere visualization"),
	ECVF_Cheat
);

UAbilityTask_GrantNearbyInteraction::UAbilityTask_GrantNearbyInteraction(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

UAbilityTask_GrantNearbyInteraction* UAbilityTask_GrantNearbyInteraction::GrantNearbyInteraction(
	UGameplayAbility* OwningAbility,
	float InteractionScanRange,
	float InteractionScanRate)
{
	UAbilityTask_GrantNearbyInteraction* MyObj = NewAbilityTask<UAbilityTask_GrantNearbyInteraction>(OwningAbility);
	MyObj->InteractionScanRange = InteractionScanRange;
	MyObj->InteractionScanRate = InteractionScanRate;
	return MyObj;
}

void UAbilityTask_GrantNearbyInteraction::Activate()
{
	SetWaitingOnAvatar();
	GetWorld()->GetTimerManager().SetTimer(QueryTimerHandle, this, &ThisClass::QueryInteractables, InteractionScanRate, true);
}

void UAbilityTask_GrantNearbyInteraction::QueryInteractables()
{
	UWorld* World = GetWorld();
	AActor* ActorOwner = GetAvatarActor();
	
	if (World && ActorOwner)
	{
		FCollisionQueryParams Params(SCENE_QUERY_STAT(UAbilityTask_GrantNearbyInteraction), false);
	
		TArray<FOverlapResult> OverlapResults;
		World->OverlapMultiByChannel(
			OUT OverlapResults,
			ActorOwner->GetActorLocation(),
			FQuat::Identity,
			ECC_Interaction,
			FCollisionShape::MakeSphere(InteractionScanRange),
			Params);

#if ENABLE_DRAW_DEBUG

		if (GCVarDrawDebugAbilityInteractionSphere)
		{
			DrawDebugSphere(
				World,
				ActorOwner->GetActorLocation(),
				InteractionScanRange,
				32,
				FColor::Yellow,
				false,     
				InteractionScanRate       
			);
		}
		
#endif
	
		if (OverlapResults.Num() > 0)
		{
			TArray<TScriptInterface<IInteractionInterface>> InteractableActors;
			UInteractionStatics::AppendInteractablesFromOverlapResult(OverlapResults, OUT InteractableActors);

			FInteractionQuery InteractionQuery;
			InteractionQuery.RequestingActor = ActorOwner;
			InteractionQuery.RequestingActor = Cast<AController>(ActorOwner->GetOwner());

			TArray<FInteractionOption> Options;
			for (TScriptInterface<IInteractionInterface>& Interactable : InteractableActors)
			{
				FInteractionOptionBuilder InteractionOptionBuilder(Interactable, Options);
				Interactable->GatherInteractionOption(InteractionQuery, InteractionOptionBuilder);
			}

			for (FInteractionOption& Option : Options)
			{
				if (Option.InteractionAbility)
				{
					FObjectKey ObjectKey(Option.InteractionAbility);
					if (!InteractionAbilityCache.Find(ObjectKey))
					{
						FGameplayAbilitySpec Spec(Option.InteractionAbility, 1, INDEX_NONE, this);
						FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
						InteractionAbilityCache.Add(ObjectKey, Handle);
					}
				}
			}
		}
	}
}

void UAbilityTask_GrantNearbyInteraction::OnDestroy(bool bInOwnerFinished)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(QueryTimerHandle);
	}
	Super::OnDestroy(bInOwnerFinished);
}


