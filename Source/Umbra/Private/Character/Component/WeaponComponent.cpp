// Copyrighted by Vorona Games


#include "Character/Component/WeaponComponent.h"
#include "GameFramework/Character.h"

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("Weapon Mesh");
}

FVector UWeaponComponent::GetBaseSocketLocation() const
{
	return WeaponMesh->GetSocketLocation(BaseSocketName);
}

FVector UWeaponComponent::GetTipSocketLocation() const
{
	return WeaponMesh->GetSocketLocation(TipSocketName);
}
