#include "BTSelectLeader.h"
#include "Gameplay/MyGameState.h"
#include "Gameplay/MyPlayerState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

UBTSelectLeader::UBTSelectLeader()
{
	NodeName = TEXT("Select Leader");

	Interval = 0.2f;
	RandomDeviation = 0.0f;

	bNotifyTick = true;

	LeaderActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTSelectLeader, LeaderActorKey), AActor::StaticClass());
}

void UBTSelectLeader::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (BB == nullptr)
	{
		return;
	}

	AMyGameState* GS = GetWorld()->GetGameState<AMyGameState>();
	if (GS == nullptr)
	{
		BB->ClearValue(LeaderActorKey.SelectedKeyName);
		return;
	}

	AMyPlayerState* Leader = GS->CurrentLeader;
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
