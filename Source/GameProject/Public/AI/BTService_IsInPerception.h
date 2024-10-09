// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_IsInPerception.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UBTService_IsInPerception : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_IsInPerception();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
