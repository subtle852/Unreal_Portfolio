// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_IsInModerateRange.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UBTService_IsInModerateRange : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_IsInModerateRange();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool IsInModerateRange(class AGMonster* Monster, class AGCharacter* Target);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IsInModerateRange", meta = (AllowPrivateAccess))
	float ModerateRange;
	
};
