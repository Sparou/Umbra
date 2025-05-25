// Copyrighted by Vorona Games


#include "Actors/Traps/UmbraTeleportTrap.h"

#include "Character/UmbraPlayerCharacter.h"
#include "GameFramework/Character.h"

void AUmbraTeleportTrap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || !InitializedCharacter || Cast<AUmbraPlayerCharacter>(OtherActor) == InitializedCharacter)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)
	{
		FVector TrapLocation = GetActorLocation();
		FVector TeleportLocation = TrapLocation + FVector(0.f, 0.f, TeleportDistance);
		Character->TeleportTo(TeleportLocation, Character->GetActorRotation(), false, true);
	}

	// Можно вызывать базовую реализацию, если нужно также применять эффекты
	Super::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}