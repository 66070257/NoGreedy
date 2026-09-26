#include "Crystal.h"
#include "NoGreedyPlayerState.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/Character.h"

ACrystal::ACrystal()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetRelativeScale3D(FVector(0.4f));

	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->SetupAttachment(Root);
	CollectionSphere->SetSphereRadius(35.0f);
	CollectionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(0.0f, 90.0f, 0.0f);
	RotatingMovement->bRotationInLocalSpace = true;

	OnActorBeginOverlap.AddDynamic(this, &ACrystal::OnBeginOverlap);
}

void ACrystal::SetDroppedBy(ANoGreedyPlayerState* Player, float LockoutSeconds)
{
	if (!HasAuthority())
	{
		return;
	}

	DroppedByPlayer = Player;
	RecollectAvailableTime = GetWorld()->GetTimeSeconds() + LockoutSeconds;
}

void ACrystal::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!HasAuthority())
	{
		return;
	}

	ACharacter* OverlappedCharacter = Cast<ACharacter>(OtherActor);
	if (!OverlappedCharacter || !OverlappedCharacter->IsPlayerControlled())
	{
		return;
	}

	ANoGreedyPlayerState* PS = OverlappedCharacter->GetPlayerState<ANoGreedyPlayerState>();
	if (!PS)
	{
		return;
	}

	if (DroppedByPlayer.IsValid() && DroppedByPlayer.Get() == PS && GetWorld()->GetTimeSeconds() < RecollectAvailableTime)
	{
		return;
	}

	if (!PS->AddCrystals(1))
	{
		return;
	}

	CollectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BP_OnCollected();

	Destroy();
}
