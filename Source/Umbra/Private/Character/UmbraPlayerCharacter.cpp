	// Copyrighted by Vorona Games


#include "Character/UmbraPlayerCharacter.h"

#include "Character/Component/InteractionComponent.h"
#include "AbilitySystem/UmbraAttributeSet.h"

AUmbraPlayerCharacter::AUmbraPlayerCharacter(const FObjectInitializer& ObjInit)
{
	AttributeSet = CreateDefaultSubobject<UUmbraAttributeSet>("Attribute Set");
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>("Interaction Component");
}

AActor* AUmbraPlayerCharacter::GetOverlappedActorFromInteractionComponent() const
{
	return InteractionComponent->OverlappedActor;
}

FStealthKillMontages AUmbraPlayerCharacter::GetRandomStealthKillMontages() const
{
	return StealthKillMontages.Num() > 0 ? StealthKillMontages[FMath::RandRange(0, StealthKillMontages.Num() - 1)] : FStealthKillMontages();
}

void AUmbraPlayerCharacter::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();
}
