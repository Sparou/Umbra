// Copyrighted by Vorona Games


#include "AnimNotify/MakeNoiseNotify.h"

void UMakeNoiseNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if(!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if(!Owner) return;

	APawn* Pawn = Cast<APawn>(Owner);
	if(!Pawn) return;
	
	Pawn->MakeNoise(Loudness, Pawn, Pawn->GetActorLocation());
}
