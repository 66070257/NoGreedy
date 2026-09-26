#include "MyPlayerState.h"
#include "MyGameState.h"
#include "Net/UnrealNetwork.h"

AMyPlayerState::AMyPlayerState()
{
}

void AMyPlayerState::AddCrystals(int32 Amount)
{
	if (!HasAuthority())
	{
		return;
	}

	CrystalCount = FMath::Max(CrystalCount + Amount, 0);

	OnRep_CrystalCount();

	if (UWorld* World = GetWorld())
	{
		if (AMyGameState* GS = World->GetGameState<AMyGameState>())
		{
			GS->RecalculateLeader();
		}
	}
}

void AMyPlayerState::OnRep_CrystalCount()
{
	BP_OnCrystalCountChanged(CrystalCount);
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyPlayerState, CrystalCount);
	DOREPLIFETIME(AMyPlayerState, bEliminated);
}
