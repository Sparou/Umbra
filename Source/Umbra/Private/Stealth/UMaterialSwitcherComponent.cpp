// Copyrighted by Vorona Games


#include "Stealth/UMaterialSwitcherComponent.h"

#include "Components/MeshComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

UMaterialSwitcherComponent::UMaterialSwitcherComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMaterialSwitcherComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMaterialSwitcherComponent::SetIsNight(bool bNight)
{
	UMaterialInterface* TargetMaterial = bNight ? NightMaterial : DayMaterial;
	if (TargetMaterial)
	{
		ApplyMaterial(TargetMaterial);
	}
}

void UMaterialSwitcherComponent::ApplyMaterial(UMaterialInterface* Material)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	if (UStaticMeshComponent* Mesh = Owner->FindComponentByClass<UStaticMeshComponent>())
	{
		Mesh->SetMaterial(MaterialIndex, Material);
	}
	else if (USkeletalMeshComponent* SkelMesh = Owner->FindComponentByClass<USkeletalMeshComponent>())
	{
		SkelMesh->SetMaterial(MaterialIndex, Material);
	}
}


