// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Character/UmbraBaseCharacter.h"
#include "UmbraPlayerCharacter.generated.h"

UENUM(BlueprintType)
enum EMovementStatus
{
	OnGround,
	InAir
};

UENUM(BlueprintType)
enum ERotationMode
{
	OrientToMovement,
	Strafe
};

UENUM(BlueprintType)
enum EMovementGait
{
	Walk,
	Run,
	Sprint
};

UENUM(BlueprintType)
enum EMovementState
{
	Idle,
	Moving
};

UENUM(BlueprintType)
enum EMovementStance
{
	Crouch,
	Stand
};

class UInteractionComponent;
/**
 * 
 */
UCLASS()
class UMBRA_API AUmbraPlayerCharacter : public AUmbraBaseCharacter
{
	GENERATED_BODY()
public:
	AUmbraPlayerCharacter(const FObjectInitializer& ObjInit);
	void Fu();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components");
	TObjectPtr<UInteractionComponent> InteractionComponent;
};
