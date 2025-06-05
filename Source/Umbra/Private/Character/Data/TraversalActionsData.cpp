// Copyrighted by Vorona Games


#include "Character/Data/TraversalActionsData.h"

FTraversalActionData UTraversalActionsData::FindActionDataByTag(FGameplayTag Action)
{
	return ActionsMap.FindRef(Action);
}
