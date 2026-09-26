#include "BTService_SelectLeader.h"
#include "Gameplay/NoGreedyGameState.h"
#include "Gameplay/NoGreedyPlayerState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

UBTService_SelectLeader::UBTService_SelectLeader()
{
	NodeName = TEXT("Select Leader");

	Interval = 0.2f;
	RandomDeviation = 0.0f;

	bNotifyTick = true;

	LeaderActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_SelectLeader, LeaderActorKey), AActor::StaticClass());
}

void UBTService_SelectLeader::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (BB == nullptr)
	{
		return;
	}

	ANoGreedyGameState* GS = GetWorld()->GetGameState<ANoGreedyGameState>();
	if (GS == nullptr)
	{
		BB->ClearValue(LeaderActorKey.SelectedKeyName);
		return;
	}

	ANoGreedyPlayerState* Leader = GS->GetCurrentLeader();
	if (!IsValid(Leader))
	{
		BB->ClearValue(LeaderActorKey.SelectedKeyName);
		return;
	}

	APawn* LeaderPawn = Leader->GetPawn();
	if (!IsValid(LeaderPawn))
	{
		BB->ClearValue(LeaderActorKey.SelectedKeyName);
		return;
	}

	BB->SetValueAsObject(LeaderActorKey.SelectedKeyName, LeaderPawn);
}
