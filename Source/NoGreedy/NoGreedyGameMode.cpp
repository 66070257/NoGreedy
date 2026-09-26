#include "NoGreedyGameMode.h"
#include "Gameplay/NoGreedyGameState.h"
#include "Gameplay/NoGreedyPlayerState.h"
#include "Gameplay/Crystal.h"
#include "EngineUtils.h"
#include "Engine/TargetPoint.h"
#include "TimerManager.h"
#include "NoGreedy.h"

void ANoGreedyGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (ANoGreedyGameState* GS = GetGameState<ANoGreedyGameState>())
	{
		GS->OnRoundOver.AddDynamic(this, &ANoGreedyGameMode::HandleRoundOver);
	}
}

void ANoGreedyGameMode::HandleRoundOver(ANoGreedyPlayerState* RoundWinner)
{
	if (RestartDelay <= 0.f)
	{
		return;
	}

	GetWorldTimerManager().SetTimer(RestartTimerHandle, this, &ANoGreedyGameMode::TravelToGame, RestartDelay, false);
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

void ANoGreedyGameMode::EliminatePlayer(AController* Victim, AController* Killer, bool bDropCrystals)
{
	if (Victim == nullptr)
	{
		return;
	}

	// Nobody can be eliminated once the round has been decided
	ANoGreedyGameState* GS = GetGameState<ANoGreedyGameState>();
	if (GS && GS->IsRoundOver())
	{
		return;
	}

	ANoGreedyPlayerState* PS = Victim->GetPlayerState<ANoGreedyPlayerState>();
	if (PS == nullptr || PS->IsEliminated())
	{
		return;
	}

	const int32 Count = PS->Eliminate();

	UE_LOG(LogNoGreedy, Warning, TEXT("ELIMINATED: %s (caught by %s) %s %d crystals"),
		*PS->GetPlayerName(), *GetNameSafe(Killer), bDropCrystals ? TEXT("dropping") : TEXT("losing"), Count);

	APawn* DeadPawn = Victim->GetPawn();
	if (bDropCrystals && DeadPawn && CrystalClass)
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

	if (DeadPawn)
	{
		DeadPawn->Destroy();
	}

	if (GS)
	{
		GS->CheckLastSurvivor();
	}
}
