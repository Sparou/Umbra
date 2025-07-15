// Copyrighted by Vorona Games

#include "Actor/UmbraAbilityInteractableActor.h"
#include "AbilitySystem/UmbraAbilitySystemComponent.h"

AUmbraAbilityInteractableActor::AUmbraAbilityInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("Ability System");
}

void AUmbraAbilityInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	AddInteractableAbilities();
}

void AUmbraAbilityInteractableActor::GatherInteractionOption(const FInteractionQuery& Query, FInteractionOptionBuilder& Builder)
{
	InteractionOption.InteractionInterface = this;
	if (AbilitySystemComponent)
	{
		InteractionOption.TargetAbilitySystemComponent = AbilitySystemComponent;
	}
	if (FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(InteractionAbility))
	{
		InteractionOption.TargetInteractionAbilitySpecHandle = Spec->Handle;
	}
	Builder.AddInteractionOption(InteractionOption);
}

void AUmbraAbilityInteractableActor::AddInteractableAbilities()
{
	FGameplayAbilitySpec AbilitySpec(InteractionAbility, 1, INDEX_NONE, this);
	AbilitySystemComponent->GiveAbility(AbilitySpec);
}

