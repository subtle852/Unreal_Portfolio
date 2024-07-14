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
	
public:
	AGMonster();
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetWidget(UGWidget* InGWidget) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION()
	virtual void OnCheckHit();
	
	virtual void DrawDetectLine(const bool bResult, FVector CenterPosition, float DetectRadius, FVector PCLocation, FVector MonsterLocation);
	
protected:
	virtual void BeginAttack();

	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGMonster|AI", meta = (AllowPrivateAccess))
	TObjectPtr<class UBlackboardData> BlackboardDataAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGMonster|AI", meta = (AllowPrivateAccess))
	TObjectPtr<class UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGMonster|Attack", meta = (AllowPrivateAccess))
	uint8 bIsNowAttacking : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGMonster|UI", meta = (AllowPrivateAccess))
	TObjectPtr<UGWidgetComponent> WidgetComponent;
	
};

