// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GAIController.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRecieveMoveCompleted);

UCLASS()
class GAMEPROJECT_API AGAIController : public AAIController
{
	GENERATED_BODY()

	friend class AGMonster;
	friend class AGBoss01;
	friend class UBTService_DetectPlayerCharacter;
	friend class UBTTask_Hover;
	friend class UBTTask_MoveToBack;
	friend class UBTTask_Teleport;
	
public:
	AGAIController();

	void InitializeAI(UBlackboardData* InBlackboardData, UBehaviorTree* InBehaviorTree);

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void BeginAI(APawn* InPawn);

	void EndAI();
	
	// Overriding base
	UPROPERTY()
	FOnRecieveMoveCompleted OnAGAIController_MoveCompleted;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override
	{
		Super::OnMoveCompleted(RequestID, Result);
		
		OnAGAIController_MoveCompleted.Broadcast();
	};
	
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

	static const FName InitialLocation;
	
	static const FName IsInModerateRangeKey;
	
	static const FName IsShoutKey;

	static const FName IsInAttackRangeKey;
	
	//static int32 ShowAIDebug;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAIController", meta = (AllowPrivateAccess))
	TObjectPtr<AActor> TargetActor;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<class UBlackboardData> BlackboardDataAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<class UBehaviorTree> BehaviorTree;

};

