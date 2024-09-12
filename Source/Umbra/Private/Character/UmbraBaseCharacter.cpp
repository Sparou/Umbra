// Copyrighted by Vorona Games


#include "Character/UmbraBaseCharacter.h"

// Sets default values
AUmbraBaseCharacter::AUmbraBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AUmbraBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUmbraBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AUmbraBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

