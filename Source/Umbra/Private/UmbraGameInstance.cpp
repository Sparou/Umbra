// Copyrighted by Vorona Games


#include "Game/UmbraGameInstance.h"

TSubclassOf<APawn> UUmbraGameInstance::GetPawn()
{
	return SelectedPawnClass;
}