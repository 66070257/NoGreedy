#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyAICharacter.generated.h"

UCLASS()
class NOGREEDY_API AMyAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyAICharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Movement")
	float PatrolSpeed = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Movement")
	float ChaseSpeed = 480.f;

	UFUNCTION(BlueprintCallable, Category = "AI|Movement")
	void SetChasing(bool bChasing);

	UPROPERTY(EditAnywhere, Category = "AI|Melee")
	float MeleeRange = 150.f;

	UPROPERTY(EditAnywhere, Category = "AI|Melee")
	float MeleeHalfAngle = 180.f;

	UPROPERTY(EditAnywhere, Category = "AI|Melee")
	float MinAttackInterval = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Melee")
	class UAnimMontage* AttackMontage;

	bool MeleeAttack();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayAttack();

private:
	float LastAttackTime = -1000.f;
};
