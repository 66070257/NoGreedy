#include "NoGreedyPlayerState.h"
#include "NoGreedyGameState.h"
#include "Net/UnrealNetwork.h"

bool ANoGreedyPlayerState::AddCrystals(int32 Amount)
{
	if (!HasAuthority() || (bEliminated && Amount > 0))
	{
		return false;
	}

	// Scores are frozen once the round has been decided
	ANoGreedyGameState* GS = GetWorld() ? GetWorld()->GetGameState<ANoGreedyGameState>() : nullptr;
	if (GS && GS->IsRoundOver())
	{
		return false;
	}

	CrystalCount = FMath::Max(CrystalCount + Amount, 0);

	OnRep_CrystalCount();

	if (GS)
	{
		GS->RecalculateLeader();
	}

	return true;
}

int32 ANoGreedyPlayerState::Eliminate()
{
	if (!HasAuthority() || bEliminated)
	{
		return 0;
	}

	// Set the flag before clearing crystals so RecalculateLeader skips this player
	bEliminated = true;

	const int32 Dropped = CrystalCount;
	AddCrystals(-Dropped);
	return Dropped;
}

void ANoGreedyPlayerState::OnRep_CrystalCount()
{
	BP_OnCrystalCountChanged(CrystalCount);
}

void ANoGreedyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANoGreedyPlayerState, CrystalCount);
	DOREPLIFETIME(ANoGreedyPlayerState, bEliminated);
}
