// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GCharacter.h"
#include "GMonster.generated.h"

class UAnimMontage;
class UGWidgetComponent;
/**
 * 
 */
UCLASS()
class GAMEPROJECT_API AGMonster : public AGCharacter
{
	GENERATED_BODY()

	friend class UBTTask_Attack;
	friend class UBTTask_Shoot;
	friend class UBTTask_MoveToBack;
	friend class UBTTask_Hover;
	friend class UBTTask_Shout;
	friend class UBTTask_Teleport;
	friend class UBTDecorator_IsInAttackRange;
	
public:
	AGMonster();
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetWidget(UGWidget* InGWidget) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION()
	virtual void OnCheckHit();

	UFUNCTION()
	virtual void OnShootProjectile();

	UFUNCTION()
	virtual void OnShootAOE();
	
	virtual void DrawDetectLine(const bool bResult, FVector CenterPosition, float DetectRadius, FVector PCLocation, FVector MonsterLocation);

	UFUNCTION()
	virtual void OnJump();
	
protected:
	virtual void BeginAttack();

	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped);

	virtual void BeginShoot();

	virtual void EndShoot(UAnimMontage* InMontage, bool bInterruped);

	virtual void Teleport();

	UFUNCTION()
	virtual void TeleportEnd();
	
	virtual void MoveToBackFromTarget(const FVector& InDirection);

	virtual void BeginShout();

	virtual void EndShout(UAnimMontage* InMontage, bool bInterruped);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGMonster|AI", meta = (AllowPrivateAccess))
	TObjectPtr<class UBlackboardData> BlackboardDataAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGMonster|AI", meta = (AllowPrivateAccess))
	TObjectPtr<class UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGMonster|Attack", meta = (AllowPrivateAccess))
	uint8 bIsNowAttacking : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGMonster|Attack", meta = (AllowPrivateAccess))
	uint8 bIsNowMovingToBackFromTarget : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGMonster|Attack", meta = (AllowPrivateAccess))
	FVector InitialLocationOfMovingToBackFromTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGMonster|Attack", meta = (AllowPrivateAccess))
	uint8 bIsNowHovering : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGMonster|UI", meta = (AllowPrivateAccess))
	TObjectPtr<UGWidgetComponent> WidgetComponent;

	FOnMontageEnded OnBasicAttackMontageEndedDelegate;
	
	FOnMontageEnded OnBasicAttackMontageEndedDelegate_Task;

	FOnMontageEnded OnShootMontageEndedDelegate;
	
	FOnMontageEnded OnShootMontageEndedDelegate_Task;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGMonster|Shout", meta = (AllowPrivateAccess))
	uint8 bIsShout : 1;

	FOnMontageEnded OnShoutMontageEndedDelegate;
	
	FOnMontageEnded OnShoutMontageEndedDelegate_Task;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGMonster|Teleport", meta = (AllowPrivateAccess))
	uint8 bIsNowTeleporting : 1;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|UI", meta = (AllowPrivateAccess))
	// TObjectPtr<class UGW_HPBar> BossHPBarWidgetRef;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|UI", meta = (AllowPrivateAccess))
	// TSubclassOf<class UGW_HPBar> BossHPBarWidgetTemplate;
	//
};

