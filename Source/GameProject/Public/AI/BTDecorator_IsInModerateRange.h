// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsInModerateRange.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UBTDecorator_IsInModerateRange : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_IsInModerateRange();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	static const float ModerateRange;
};
