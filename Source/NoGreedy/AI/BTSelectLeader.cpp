// No Greedy! game project

#include "BTSelectLeader.h"
#include "Gameplay/MyGameState.h"
#include "Gameplay/MyPlayerState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

UBTSelectLeader::UBTSelectLeader()
{
	NodeName = TEXT("Select Leader");

	// อ่านทุก 0.2 วิ ถี่กว่านี้ไม่มีประโยชน์ เพราะเป้าถูกล็อกไว้ 1.0 วิอยู่แล้ว
	Interval = 0.2f;
	RandomDeviation = 0.0f;

	bNotifyTick = true;

	// ให้ดรอปดาวน์ในทรีโชว์เฉพาะคีย์ชนิด Object ที่เป็น Actor
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

	// null-check สามชั้น: GameState -> PlayerState ของคนนำ -> pawn ของเขา
	// คนนำอาจเพิ่งตกรอบหรือ pawn เพิ่งถูกสร้างใหม่ตอนรีเซ็ตรอบ ในเฟรมเดียวกับที่ service ทำงาน
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

	// เขียน pawn ลงคีย์ ไม่ใช่ PlayerState -- MoveTo ต้องการตัวที่มีตำแหน่งในโลก
	APawn* LeaderPawn = Leader->GetPawn();
	if (!IsValid(LeaderPawn))
	{
		BB->ClearValue(LeaderActorKey.SelectedKeyName);
		return;
	}

	BB->SetValueAsObject(LeaderActorKey.SelectedKeyName, LeaderPawn);
}
