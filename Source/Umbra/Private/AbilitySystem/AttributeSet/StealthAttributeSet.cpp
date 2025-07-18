// Copyrighted by Vorona Games


#include "StealthAttributeSet.h"

#include "Net/UnrealNetwork.h"

void UStealthAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UStealthAttributeSet, Visibility, COND_None, REPNOTIFY_Always)
}
