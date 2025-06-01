	// Copyrighted by Vorona Games


#include "Character/UmbraPlayerCharacter.h"
#include "Character/Component/InteractionComponent.h"
#include "TraversalSystem/TraversalComponent.h"
#include "AbilitySystem/UmbraAttributeSet.h"
#include "Stealth/LightingDetection.h"
#include "Blueprint/UserWidget.h"
#include "Stealth/LightLevelIndicator.h"
#include "Umbra/Umbra.h"
#include "Stealth/LightLevelIndicator.h"

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
	if (LightWidgetClass)
	{
		// Создаём и показываем наш C++-виджет
		ULightLevelIndicator* Widget = CreateWidget<ULightLevelIndicator>(GetWorld(), LightWidgetClass);
		if (Widget)
		{
			Widget->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create LightLevelIndicator widget"));
		}
	}
}

void AUmbraPlayerCharacter::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();
}
