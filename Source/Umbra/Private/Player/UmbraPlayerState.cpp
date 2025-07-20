// Copyrighted by Vorona Games


#include "Player/UmbraPlayerState.h"

#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSet/MovementAttributeSet.h"
#include "AbilitySystem/AttributeSet/StealthAttributeSet.h"
#include "AbilitySystem/AttributeSet/VitalAttributeSet.h"
#include "Net/UnrealNetwork.h"

AUmbraPlayerState::AUmbraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UUmbraAbilitySystemComponent>("Ability System Component");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	VitalAttributeSet = CreateDefaultSubobject<UVitalAttributeSet>("Vital Attributes");
	MovementAttributeSet = CreateDefaultSubobject<UMovementAttributeSet>("Movement Attributes");
	StealthAttributeSet = CreateDefaultSubobject<UStealthAttributeSet>("Stealth Attributes");
	
	SetNetUpdateFrequency(100.f);
}

void AUmbraPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUmbraPlayerState, Level);
}

