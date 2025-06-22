// Copyrighted by Vorona Games


#include "Player/UmbraPlayerState.h"

#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "AbilitySystem/UmbraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AUmbraPlayerState::AUmbraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UUmbraAbilitySystemComponent>("Ability System Component");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UUmbraAttributeSet>("Attribute Set");
	
	SetNetUpdateFrequency(100.f);
}

UUmbraAbilitySystemComponent* AUmbraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UUmbraAttributeSet* AUmbraPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

int16 AUmbraPlayerState::GetPlayerLevel() const
{
	return Level;
}

void AUmbraPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AUmbraPlayerState, Level);
}
