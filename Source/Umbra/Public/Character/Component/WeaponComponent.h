// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UMBRA_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

	UStaticMeshComponent* GetWeaponMesh() const { return WeaponMesh; };
	FName GetCharacterSocketName() const { return CharacterSocketName; };
	FName GetBaseSocketName() const { return BaseSocketName; };
	FName GetTipSocketName() const { return TipSocketName; };

	FVector GetBaseSocketLocation() const;
	FVector GetTipSocketLocation() const;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName CharacterSocketName = "RWeaponSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName BaseSocketName = FName("WeaponBaseSocket");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TipSocketName = FName("WeaponTipSocket");

private:

	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;
};
