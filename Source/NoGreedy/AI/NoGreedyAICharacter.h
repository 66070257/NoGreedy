#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NoGreedyAICharacter.generated.h"

class UAnimMontage;
class UNiagaraSystem;
class USoundBase;

/**
 *  Hunter AI. Chases the crystal leader (see UBTService_SelectLeader) and eliminates
 *  every player inside MeleeRange when it attacks. Driven by ANoGreedyAIController.
 */
UCLASS()
class NOGREEDY_API ANoGreedyAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ANoGreedyAICharacter();

	/** Applied as MaxWalkSpeed in the constructor */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Movement")
	float PatrolSpeed = 250.f;

	/** Any player pawn closer than this is eliminated by an attack */
	UPROPERTY(EditAnywhere, Category = "AI|Melee")
	float MeleeRange = 150.f;

	/** Minimum seconds between two successful attacks */
	UPROPERTY(EditAnywhere, Category = "AI|Melee")
	float MinAttackInterval = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Melee")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Melee")
	TObjectPtr<UNiagaraSystem> HitEffect;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Melee")
	TObjectPtr<USoundBase> HitSound;

	/**
	 *  Server only. Eliminates all players in MeleeRange and plays the attack on every machine.
	 *  Does nothing (returns false) on cooldown, after the round ends, when there is no leader, or when nobody is in range.
	 */
	bool MeleeAttack();

	// Every machine spawns its own effect and sound at the caught player's location
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayAttack(FVector_NetQuantize HitLocation);

private:
	/** Server only. World time of the last successful attack */
	float LastAttackTime = -1000.f;
};
