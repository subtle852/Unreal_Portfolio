// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GetRandNum.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GMonster.h"
#include "Controller/GAIController.h"

UBTTask_GetRandNum::UBTTask_GetRandNum()
{
	NodeName = TEXT("GetRandNum");
	bNotifyTick = true;

	NumType = ERandNumType::None;
	MinNum = 0;
	MaxNum = 10;
}

void UBTTask_GetRandNum::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EBTNodeResult::Type UBTTask_GetRandNum::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (EBTNodeResult::Failed == Result)
	{
		return Result;
	}

	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));

	AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));

	int32 RandNum = FMath::RandRange(MinNum,MaxNum);
	
	if(NumType == ERandNumType::NormalRand)
	{
		
	}
	else if(NumType == ERandNumType::PatternAttack)
	{
		// Queue 저장
	}
	else
	{
		
	}
	OwnerComp.GetBlackboardComponent()->SetValueAsInt(AGAIController::PatternAttackRandNumKey, RandNum);

	return Result = EBTNodeResult::Succeeded;
}
