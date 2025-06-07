// Copyrighted by Vorona Games


#include "UmbraGameInstance.h"

TSubclassOf<APawn> UUmbraGameInstance::GetPawn()
{
	return SelectedPawnClass;
}