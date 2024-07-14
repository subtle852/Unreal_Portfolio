// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetEndPatrolPosition.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UBTTask_GetEndPatrolPosition : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_GetEndPatrolPosition();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

