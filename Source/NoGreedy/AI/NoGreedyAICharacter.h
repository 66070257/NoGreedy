#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NoGreedyAICharacter.generated.h"

class UAnimMontage;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class NOGREEDY_API ANoGreedyAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ANoGreedyAICharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Movement")
	float PatrolSpeed = 250.f;

	UPROPERTY(EditAnywhere, Category = "AI|Melee")
	float MeleeRange = 150.f;

	UPROPERTY(EditAnywhere, Category = "AI|Melee")
	float MinAttackInterval = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Melee")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Melee")
	TObjectPtr<UNiagaraSystem> HitEffect;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Melee")
	TObjectPtr<USoundBase> HitSound;

	bool MeleeAttack();

	// Every machine spawns its own effect and sound at the caught player's location
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayAttack(FVector_NetQuantize HitLocation);

private:
	float LastAttackTime = -1000.f;
};
