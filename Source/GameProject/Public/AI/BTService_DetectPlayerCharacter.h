// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DetectPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UBTService_DetectPlayerCharacter : public UBTService
{
	GENERATED_BODY()
	
	public:
    	UBTService_DetectPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    
    protected:
    	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DetectPlayerCharacter", meta = (AllowPrivateAccess))
	float DetectRadius;
	
};

