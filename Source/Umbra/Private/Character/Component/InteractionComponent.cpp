// Copyrighted by Vorona Games


#include "Character/Component/InteractionComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "UmbraGameplayTags.h"
#include "Character/UmbraBaseCharacter.h"
#include "Components/BoxComponent.h"


UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner())
	{
		InteractionCollisionComponent = GetOwner()->GetComponentByClass<UBoxComponent>();

		checkf(InteractionCollisionComponent, TEXT("Interaction Collision Component is not added to [%s]"), *GetOwner()->GetName());
		
		InteractionCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &UInteractionComponent::OnBeginOverlap);
		InteractionCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &UInteractionComponent::OnEndOverlap);
	}
}


void UInteractionComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OverlappedActor = OtherActor;
	if (OtherActor->ActorHasTag("Character"))
	{
		//TryToStealthKill(OtherActor);
	}
}

void UInteractionComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappedActor = nullptr;
}

void UInteractionComponent::TryToStealthKill(AActor* Target)
{
	UAbilitySystemComponent* InstigatorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);

	if (InstigatorASC && TargetASC)
	{
		FUmbraGameplayTags GameplayTags = FUmbraGameplayTags::Get();
		FGameplayTagContainer KillerTags = FGameplayTagContainer(GameplayTags.Ability_Stealth_Kill);
		FGameplayTagContainer VictimTags = FGameplayTagContainer(GameplayTags.Ability_Stealth_Victim);
		InstigatorASC->TryActivateAbilitiesByTag(KillerTags);
		TargetASC->TryActivateAbilitiesByTag(VictimTags);
	}
}
