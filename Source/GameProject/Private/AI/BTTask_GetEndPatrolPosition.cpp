// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GetEndPatrolPosition.h"
#include "Controller/GAIController.h"
#include "Character/GMonster.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GetEndPatrolPosition::UBTTask_GetEndPatrolPosition()
{
	NodeName = TEXT("GetEndPatrolPosition"); // Behavior Tree에 보일 노드 이름
}

EBTNodeResult::Type UBTTask_GetEndPatrolPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(Monster->GetWorld());
	ensureMsgf(IsValid(NavigationSystem), TEXT("Invalid NavigationSystem"));

	FVector StartPatrolPosition = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AGAIController::StartPatrolPositionKey);
	FNavLocation EndPatrolLocation;
	if (true == NavigationSystem->GetRandomPointInNavigableRadius(StartPatrolPosition, AIController->PatrolRadius, EndPatrolLocation))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AGAIController::EndPatrolPositionKey, EndPatrolLocation.Location);
		return Result = EBTNodeResult::Succeeded;
	}

	return Result;
}

