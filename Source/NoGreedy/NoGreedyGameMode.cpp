#include "NoGreedyGameMode.h"
#include "Gameplay/MyGameState.h"
#include "Gameplay/MyPlayerState.h"
#include "Gameplay/Crystal.h"
#include "EngineUtils.h"
#include "Engine/TargetPoint.h"

ANoGreedyGameMode::ANoGreedyGameMode()
{
}

void ANoGreedyGameMode::TravelToGame()
{
	GetWorld()->ServerTravel(TEXT("/Game/Maps/Game?listen"));
}

AActor* ANoGreedyGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> SpawnPoints;
	for (TActorIterator<ATargetPoint> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(PlayerSpawnTag))
		{
			SpawnPoints.Add(*It);
		}
	}

	if (SpawnPoints.Num() == 0)
	{
		return Super::ChoosePlayerStart_Implementation(Player);
	}

	SpawnPoints.Sort([](const AActor& A, const AActor& B) { return A.GetName() < B.GetName(); });

	int32 Index = 0;
	if (GameState && Player)
	{
		Index = FMath::Max(GameState->PlayerArray.IndexOfByKey(Player->PlayerState), 0);
	}

	return SpawnPoints[Index % SpawnPoints.Num()];
}

void ANoGreedyGameMode::EliminatePlayer(AController* Victim, AController* Killer)
{
	if (Victim == nullptr)
	{
		return;
	}

	AMyPlayerState* PS = Victim->GetPlayerState<AMyPlayerState>();
	if (PS == nullptr || PS->bEliminated)
	{
		return;
	}

	PS->bEliminated = true;

	UE_LOG(LogTemp, Warning, TEXT("ELIMINATED: %s (caught by %s) dropping %d crystals"),
		*PS->GetPlayerName(), *GetNameSafe(Killer), PS->CrystalCount);

	APawn* DeadPawn = Victim->GetPawn();
	const int32 Count = PS->CrystalCount;
	if (DeadPawn && CrystalClass)
	{
		const FVector Center = DeadPawn->GetActorLocation();
		for (int32 i = 0; i < Count; ++i)
		{
			const FVector2D Offset = FMath::RandPointInCircle(ScatterRadius);
			const FVector SpawnLocation = Center + FVector(Offset.X, Offset.Y, 0.f);

			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			GetWorld()->SpawnActor<ACrystal>(CrystalClass, SpawnLocation, FRotator::ZeroRotator, Params);
		}
	}

	PS->AddCrystals(-Count);

	if (DeadPawn)
	{
		DeadPawn->Destroy();
	}

	if (AMyGameState* GS = GetGameState<AMyGameState>())
	{
		GS->CheckLastSurvivor();
	}
}
