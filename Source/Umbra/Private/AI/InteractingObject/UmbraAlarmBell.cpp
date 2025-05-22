// Copyrighted by Vorona Games


#include "AI/InteractingObject/UmbraAlarmBell.h"

AUmbraAlarmBell::AUmbraAlarmBell()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AUmbraAlarmBell::BeginPlay()
{
	Super::BeginPlay();
}

void AUmbraAlarmBell::RingBell() const
{
	OnBellRang.Broadcast(GetActorLocation());	
}