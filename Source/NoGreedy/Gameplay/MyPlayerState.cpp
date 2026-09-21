// No Greedy! game project

#include "MyPlayerState.h"
#include "Net/UnrealNetwork.h"

AMyPlayerState::AMyPlayerState()
{
}

void AMyPlayerState::AddCrystals(int32 Amount)
{
	// only the server may change the authoritative count
	if (!HasAuthority())
	{
		return;
	}

	CrystalCount = FMath::Max(CrystalCount + Amount, 0);

	// RepNotify does not fire locally on the server, so call it explicitly
	OnRep_CrystalCount();
}

void AMyPlayerState::OnRep_CrystalCount()
{
	BP_OnCrystalCountChanged(CrystalCount);
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyPlayerState, CrystalCount);
}
