	// Copyrighted by Vorona Games


#include "Character/UmbraPlayerCharacter.h"

#include "Character/Component/InteractionComponent.h"
#include "AbilitySystem/UmbraAttributeSet.h"
#include "Umbra/Umbra.h"

	AUmbraPlayerCharacter::AUmbraPlayerCharacter(const FObjectInitializer& ObjInit)
{
	AttributeSet = CreateDefaultSubobject<UUmbraAttributeSet>("Attribute Set");
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>("Interaction Component");
}

AActor* AUmbraPlayerCharacter::GetOverlappedActorFromInteractionComponent() const
{
	return InteractionComponent->GetInteractionActor();
}

FStealthKillMontages AUmbraPlayerCharacter::GetRandomStealthKillMontages(TArray<FStealthKillMontages> KillMontages) const
{
	return KillMontages.Num() > 0 ? KillMontages[FMath::RandRange(0, KillMontages.Num() - 1)] : FStealthKillMontages();
}

FStealthKillMontages AUmbraPlayerCharacter::GetStealthKillMontageForPosition(EStealthKillPosition KillPosition)
{
	switch (KillPosition)
	{
	case EStealthKillPosition::Behind:
		return GetRandomStealthKillMontages(StealthKillMontagesFromBehind);
	
	case EStealthKillPosition::Front:
		return GetRandomStealthKillMontages(StealthKillMontagesFromFront);
	
	case EStealthKillPosition::Top:
		return GetRandomStealthKillMontages(StealthKillMontagesFromTop);
	
	case EStealthKillPosition::Ledge:
		return GetRandomStealthKillMontages(StealthKillMontagesFromLedge);
	
	case EStealthKillPosition::Left:
		return GetRandomStealthKillMontages(StealthKillMontagesFromLeft);
	
	case EStealthKillPosition::Right:
		return GetRandomStealthKillMontages(StealthKillMontagesFromRight);
	
	default:
		UE_LOG(LogTemp, Warning, TEXT("Unknown KillPosition: %d"), static_cast<int32>(KillPosition));
		return FStealthKillMontages();
	}
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
