	// Copyrighted by Vorona Games


#include "Character/UmbraPlayerCharacter.h"

#include "Character/Component/InteractionComponent.h"
#include "AbilitySystem/UmbraAttributeSet.h"

AUmbraPlayerCharacter::AUmbraPlayerCharacter(const FObjectInitializer& ObjInit)
{
	AttributeSet = CreateDefaultSubobject<UUmbraAttributeSet>("Attribute Set");
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>("Interaction Component");
}

void AUmbraPlayerCharacter::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();
}
