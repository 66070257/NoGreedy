#include "NoGreedyFallDeathVolume.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "NoGreedyGameMode.h"

ANoGreedyFallDeathVolume::ANoGreedyFallDeathVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	KillBox = CreateDefaultSubobject<UBoxComponent>(TEXT("KillBox"));
	RootComponent = KillBox;
	KillBox->SetBoxExtent(FVector(1000.f, 1000.f, 100.f));
	KillBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	KillBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	KillBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	OnActorBeginOverlap.AddDynamic(this, &ANoGreedyFallDeathVolume::OnBeginOverlap);
}

void ANoGreedyFallDeathVolume::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!HasAuthority())
	{
		return;
	}

	APawn* FallenPawn = Cast<APawn>(OtherActor);
	if (FallenPawn == nullptr)
	{
		return;
	}

	if (ANoGreedyGameMode* GM = GetWorld()->GetAuthGameMode<ANoGreedyGameMode>())
	{
		GM->EliminatePlayer(FallenPawn->GetController(), nullptr, false);
	}
}
