// Copyrighted by Vorona Games


#include "TraversalSystem/TraversalActionsData.h"

FTraversalActionData UTraversalActionsData::FindActionDataByTag(FGameplayTag Action)
{
	return ActionsMap.FindRef(Action);
}
