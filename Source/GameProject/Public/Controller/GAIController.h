// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GAIController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API AGAIController : public AAIController
{
	GENERATED_BODY()

	friend class AGMonster;

public:
	AGAIController();

	void InitializeAI(UBlackboardData* InBlackboardData, UBehaviorTree* InBehaviorTree);

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void BeginAI(APawn* InPawn);

	void EndAI();

//private:
//	void OnPatrolTimerElapsed();
//
//public:
//	FTimerHandle PatrolTimerHandle = FTimerHandle();
//
//	static const float PatrolRepeatInterval;
//
//	static const float PatrolRadius;

public:
	static const float PatrolRadius;

	static const FName StartPatrolPositionKey;

	static const FName EndPatrolPositionKey;

	static const FName TargetActorKey;

	//static int32 ShowAIDebug;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<class UBlackboardData> BlackboardDataAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<class UBehaviorTree> BehaviorTree;

};
