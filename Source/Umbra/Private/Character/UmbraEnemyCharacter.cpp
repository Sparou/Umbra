// Copyrighted by Vorona Games


#include "Character/UmbraEnemyCharacter.h"

void AUmbraEnemyCharacter::Interact(AActor* OtherActor)
{
	bInInteraction = true;
	GEngine->AddOnScreenDebugMessage(0, 3.f, FColor::Blue, FString("Interacted with object"));
	bInInteraction = false;
}
