#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NoGreedyGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class ANoGreedyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	ANoGreedyGameMode();

	/** SERVER ONLY -- moves the host from the Lobby to the Game map; every client follows automatically */
	UFUNCTION(BlueprintCallable, Category="Rules")
	void TravelToGame();

	/**
	 * SERVER ONLY -- ผู้เล่นโดน AI จับ (เรียกจาก AMyAICharacter::MeleeAttack)
	 * ตกรอบ -> คริสตอลกระจายลงสนาม -> ลบร่าง -> ให้ GameState เลือกคนนำใหม่ทันที
	 */
	void EliminatePlayer(AController* Victim, AController* Killer);

protected:

	/**
	 * ให้ผู้เล่นเกิดที่ TargetPoint ที่ติด Tag = PlayerSpawnTag (PlayerSpanwPoint, PlayerSpanwPoint2)
	 * host ได้จุดแรก คนที่สองได้จุดถัดไป -- ไม่มีจุดติด Tag (เช่น Lobby) = ใช้ PlayerStart ตามปกติ
	 */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/** Tag ที่ต้องใส่ใน Details -> Actor -> Tags ของ TargetPoint จุดเกิดผู้เล่น */
	UPROPERTY(EditDefaultsOnly, Category="Rules")
	FName PlayerSpawnTag = TEXT("PlayerSpawn");

	/** คริสตอลที่จะโปรยตอนมีคนโดนจับ -- ตั้งเป็น BP_Crystal ใน BP_ThirdPersonGameMode (ว่าง = คริสตอลหายไปเฉย ๆ) */
	UPROPERTY(EditDefaultsOnly, Category="Rules")
	TSubclassOf<class ACrystal> CrystalClass;

	/** รัศมีที่คริสตอลกระจายรอบจุดที่โดนจับ */
	UPROPERTY(EditDefaultsOnly, Category="Rules")
	float ScatterRadius = 300.f;
};

