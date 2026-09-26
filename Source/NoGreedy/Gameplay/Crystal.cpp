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

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(0.0f, 90.0f, 0.0f);
	RotatingMovement->bRotationInLocalSpace = true;

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

	if (DroppedByPlayer.IsValid() && DroppedByPlayer.Get() == PS && GetWorld()->GetTimeSeconds() < RecollectAvailableTime)
	{
		return;
	}

	PS->AddCrystals(1);

	CollectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BP_OnCollected();

	Destroy();
}
