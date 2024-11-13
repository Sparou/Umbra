	// Copyrighted by Vorona Games


#include "Character/UmbraPlayerCharacter.h"

#include "Character/Component/InteractionComponent.h"
#include "Components/BoxComponent.h"

AUmbraPlayerCharacter::AUmbraPlayerCharacter(const FObjectInitializer& ObjInit)
{
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>("Interaction Component");
}

void AUmbraPlayerCharacter::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();
}
