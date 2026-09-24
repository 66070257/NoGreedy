// Copyright Epic Games, Inc. All Rights Reserved.

#include "NoGreedyGameMode.h"
#include "Gameplay/MyGameState.h"
#include "Gameplay/MyPlayerState.h"
#include "Gameplay/Crystal.h"
#include "EngineUtils.h"
#include "Engine/TargetPoint.h"

ANoGreedyGameMode::ANoGreedyGameMode()
{
	// stub
}

void ANoGreedyGameMode::TravelToGame()
{
	// Keep ?listen -- without it the host stops listening and clients cannot follow.
	GetWorld()->ServerTravel(TEXT("/Game/Maps/Game?listen"));
}

AActor* ANoGreedyGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// รวม TargetPoint ที่ติด Tag จุดเกิดผู้เล่น
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

	// เรียงตามชื่อให้ลำดับคงที่ แล้วแจกจุดตามลำดับผู้เล่นใน PlayerArray (0 = host)
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

	// คริสตอลที่ถือกระจายลงสนามรอบจุดที่โดนจับ
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

	// AddCrystals เรียก RecalculateLeader ให้เองเสมอ (ถึงจะ -0) -> AI เปลี่ยนเป้าทันที ข้ามคน bEliminated
	PS->AddCrystals(-Count);

	// ลบร่างแบบ Lab -- กล้องค้างที่จุดตาย ไม่ respawn เพราะตกรอบแล้ว
	if (DeadPawn)
	{
		DeadPawn->Destroy();
	}

	// เหลือผู้เล่นรอดคนเดียว = ชนะทันที (กฎคนนำโดนล่า)
	if (AMyGameState* GS = GetGameState<AMyGameState>())
	{
		GS->CheckLastSurvivor();
	}
}
