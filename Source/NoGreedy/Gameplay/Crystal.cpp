// No Greedy! game project

#include "Crystal.h"
#include "MyPlayerState.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/Character.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

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

	// placeholder look -- swap for the real crystal mesh/material later
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaceholderMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (PlaceholderMeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(PlaceholderMeshAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> PlaceholderMaterialAsset(TEXT("/Game/Variant_Combat/Materials/M_Lava.M_Lava"));
	if (PlaceholderMaterialAsset.Succeeded())
	{
		Mesh->SetMaterial(0, PlaceholderMaterialAsset.Object);
	}

	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->SetupAttachment(Root);
	CollectionSphere->SetSphereRadius(35.0f);
	CollectionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// idle spin -- cosmetic only, runs independently on every machine (server and each client)
	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(0.0f, 90.0f, 0.0f);
	RotatingMovement->bRotationInLocalSpace = true;
	// component is not attached to a scene component on purpose -- it drives the owning
	// actor's rotation directly, so it will spin Mesh + CollectionSphere together via Root

	OnActorBeginOverlap.AddDynamic(this, &ACrystal::OnBeginOverlap);
}

void ACrystal::BeginPlay()
{
	Super::BeginPlay();
}

void ACrystal::SetDroppedBy(AMyPlayerState* Player, float LockoutSeconds)
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
	// collection is decided on the server only -- clients just see the actor vanish when it replicates away
	if (!HasAuthority())
	{
		return;
	}

	ACharacter* OverlappedCharacter = Cast<ACharacter>(OtherActor);
	if (!OverlappedCharacter || !OverlappedCharacter->IsPlayerControlled())
	{
		return;
	}

	AMyPlayerState* PS = OverlappedCharacter->GetPlayerState<AMyPlayerState>();
	if (!PS)
	{
		return;
	}

	// the player who just dropped this crystal can't instantly re-collect it
	if (DroppedByPlayer.IsValid() && DroppedByPlayer.Get() == PS && GetWorld()->GetTimeSeconds() < RecollectAvailableTime)
	{
		return;
	}

	PS->AddCrystals(1);

	// stop this crystal from being collected twice while BP_OnCollected/Destroy are in flight
	CollectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BP_OnCollected();

	Destroy();
}
