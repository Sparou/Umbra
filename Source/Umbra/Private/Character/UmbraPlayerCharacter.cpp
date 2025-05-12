	// Copyrighted by Vorona Games


#include "Character/UmbraPlayerCharacter.h"
#include "Character/Component/InteractionComponent.h"
#include "TraversalSystem/TraversalComponent.h"
#include "AbilitySystem/UmbraAttributeSet.h"
#include "Umbra/Umbra.h"

AUmbraPlayerCharacter::AUmbraPlayerCharacter(const FObjectInitializer& ObjInit)
{
	AttributeSet = CreateDefaultSubobject<UUmbraAttributeSet>("Attribute Set");
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>("Interaction Component");
	TraversalComponent = CreateDefaultSubobject<UTraversalComponent>("Traversal Component");
}

UAssassinationsData* AUmbraPlayerCharacter::GetAssassinationsData()
{
	return AssassinationsData;
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
}

void AUmbraPlayerCharacter::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();
}
