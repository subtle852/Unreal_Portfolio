// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GCharacter.h"
#include "GMonster.generated.h"

UENUM(BlueprintType)
enum class EMonsterAttackType : uint8
{
	None,
	BasicAttack,
	PatternAttack,
	End,
};

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
	friend class UBTTask_JumpAttack;
	friend class UBTTask_Shoot;
	friend class UBTTask_ShootWind;
	friend class UBTTask_ShootMultiple;
	friend class UBTTask_ShootAOE;
	friend class UBTTask_ShootLaser;
	friend class UBTTask_ShootTorus;
	friend class UBTTask_MoveToBack;
	friend class UBTTask_Hover;
	friend class UBTTask_Shout;
	friend class UBTTask_Teleport;
	friend class UBTTask_FocusTarget;
	friend class UBTTask_TurnToTarget;
	friend class UBTDecorator_IsInAttackRange;
	
public:
	AGMonster();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetWidget(UGWidget* InGWidget) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION()
	virtual void OnCheckHit();

	UFUNCTION()
	virtual void OnCheckHitDown();

	UFUNCTION()
	virtual void OnShootProjectile();

	UFUNCTION()
	virtual void OnShootWindProjectile();

	UFUNCTION()
	virtual void OnShootMultipleProjectile();
	
	UFUNCTION()
	virtual void OnShootAOE();
	
	virtual void DrawDetectLine(const bool bResult, FVector CenterPosition, float DetectRadius, FVector PCLocation, FVector MonsterLocation);

	UFUNCTION()
	virtual void OnShootShapeAOE();

	UFUNCTION()
	virtual void OnShootLaser();

	UFUNCTION()
	virtual void OnShootTorus();
	
	UFUNCTION()
	virtual void OnJump();

	UFUNCTION()
	virtual void OnStartLying();
	
protected:
	virtual void BeginAttack();

	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped);

	virtual void BeginJumpAttack();

	virtual void EndJumpAttack(UAnimMontage* InMontage, bool bInterruped);

	virtual void BeginShoot();

	virtual void EndShoot(UAnimMontage* InMontage, bool bInterruped);

	virtual void BeginShootWind();

	virtual void EndShootWind(UAnimMontage* InMontage, bool bInterruped);

	virtual void BeginShootMultiple();

	virtual void EndShootMultiple(UAnimMontage* InMontage, bool bInterruped);

	virtual void BeginShootAOE();

	virtual void EndShootAOE(UAnimMontage* InMontage, bool bInterruped);

	virtual void BeginShootLaser();

	virtual void EndShootLaser(UAnimMontage* InMontage, bool bInterruped);
	
	virtual void BeginShootTorus();

	virtual void EndShootTorus(UAnimMontage* InMontage, bool bInterruped);
	

	virtual void Teleport();

	UFUNCTION()
	virtual void TeleportEnd();

	virtual void BeginShout();

	virtual void EndShout(UAnimMontage* InMontage, bool bInterruped);

	UFUNCTION()
	void OnMonsterDeath();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGMonster|AI", meta = (AllowPrivateAccess))
	TObjectPtr<class UBlackboardData> BlackboardDataAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGMonster|AI", meta = (AllowPrivateAccess))
	TObjectPtr<class UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGMonster|Weapon", meta = (AllowPrivateAccess))
	TObjectPtr<class UStaticMeshComponent> WeaponMeshComponent;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "AGMonster|Attack", meta = (AllowPrivateAccess))
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

	FOnMontageEnded OnJumpAttackMontageEndedDelegate;
	
	FOnMontageEnded OnJumpAttackMontageEndedDelegate_Task;

	FOnMontageEnded OnShootMontageEndedDelegate;
	
	FOnMontageEnded OnShootMontageEndedDelegate_Task;

	FOnMontageEnded OnShootWindMontageEndedDelegate;
	
	FOnMontageEnded OnShootWindMontageEndedDelegate_Task;

	FOnMontageEnded OnShootMultipleMontageEndedDelegate;
	
	FOnMontageEnded OnShootMultipleMontageEndedDelegate_Task;

	FOnMontageEnded OnShootAOEMontageEndedDelegate;

	FOnMontageEnded OnShootAOEMontageEndedDelegate_Task;

	FOnMontageEnded OnShootLaserFinishMontageEndedDelegate;

	FOnMontageEnded OnShootLaserFinishMontageEndedDelegate_Task;

	FOnMontageEnded OnShootTorusMontageEndedDelegate;

	FOnMontageEnded OnShootTorusMontageEndedDelegate_Task;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGMonster|Shout", meta = (AllowPrivateAccess))
	uint8 bIsShout : 1;

	FOnMontageEnded OnShoutMontageEndedDelegate;
	
	FOnMontageEnded OnShoutMontageEndedDelegate_Task;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGMonster|Teleport", meta = (AllowPrivateAccess))
	uint8 bIsNowTeleporting : 1;

	// Attack
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "AGMonster|Attack", meta = (AllowPrivateAccess))
	EMonsterAttackType MonsterAttackType;
	
	// HitReact
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "AGMonster|HitReact", meta = (AllowPrivateAccess))
	uint8 bIsStunning : 1;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "AGMonster|HitReact", meta = (AllowPrivateAccess))
	uint8 bIsKnockDowning : 1;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "AGMonster|HitReact", meta = (AllowPrivateAccess))
	uint8 bIsAirBounding : 1;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "AGMonster|HitReact", meta = (AllowPrivateAccess))
	uint8 bIsGroundBounding : 1;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "AGMonster|HitReact", meta = (AllowPrivateAccess))
	uint8 bIsLying : 1;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "AGMonster|HitReact", meta = (AllowPrivateAccess))
	uint8 bIsHitReactTransitioning : 1;

	FOnMontageEnded OnHitReactStunMontageEndedDelegate;

	FOnMontageEnded OnHitReactKnockDownMontageEndedDelegate;

	FOnMontageEnded OnHitReactAirBoundMontageEndedDelegate;

	FOnMontageEnded OnHitReactGroundBoundMontageEndedDelegate;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|UI", meta = (AllowPrivateAccess))
	// TObjectPtr<class UGW_HPBar> BossHPBarWidgetRef;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|UI", meta = (AllowPrivateAccess))
	// TSubclassOf<class UGW_HPBar> BossHPBarWidgetTemplate;
	//
};

