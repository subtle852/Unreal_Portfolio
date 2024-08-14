// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_IsInAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UBTService_IsInAttackRange : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_IsInAttackRange();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool IsInAttackRange(class AGMonster* Monster, class AGCharacter* Target);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IsInAttackRange", meta = (AllowPrivateAccess))
	float AttackRange;
	
};
