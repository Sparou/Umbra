// Copyrighted by Vorona Games


#include "Actor/Traps/UmbraTeleportTrap.h"
#include "GameFramework/Character.h"


// void AUmbraTeleportTrap::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// 	if (UMaterialInstanceDynamic* MaterialInstance = Plane->CreateAndSetMaterialInstanceDynamic(0))
// 	{
// 		static float DissolveValue = -0.5f;
// 		static float DissolveSpeed = 0.01f;
// 		static bool bIncreasing = true;
//
// 		// Update DissolveValue
// 		if (bIncreasing)
// 		{
// 			DissolveValue += DissolveSpeed;
// 			if (DissolveValue >= 0.5f)
// 			{
// 				DissolveValue = 0.5f;
// 				bIncreasing = false;
// 			}
// 		}
// 		else
// 		{
// 			DissolveValue -= DissolveSpeed;
// 			if (DissolveValue <= -0.5f)
// 			{
// 				DissolveValue = -0.5f;
// 				bIncreasing = true;
// 			}
// 		}
// 		
// 		MaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
// 	}
// }

void AUmbraTeleportTrap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
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