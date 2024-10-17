// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DetectAnotherTarget.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UBTTask_DetectAnotherTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_DetectAnotherTarget();

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DetectPlayerCharacter", meta = (AllowPrivateAccess))
	float DetectRadius;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DetectPlayerCharacter", meta = (AllowPrivateAccess))
	// uint8 bIsDetectAnotherTarget;
};
