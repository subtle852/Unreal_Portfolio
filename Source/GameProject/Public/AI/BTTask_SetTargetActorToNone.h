// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetTargetActorToNone.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UBTTask_SetTargetActorToNone : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_SetTargetActorToNone();

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
