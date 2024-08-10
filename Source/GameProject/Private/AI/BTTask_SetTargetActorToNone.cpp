// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_SetTargetActorToNone.h"
#include "Controller/GAIController.h"
#include "Character/GMonster.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetTargetActorToNone::UBTTask_SetTargetActorToNone()
{
	NodeName = TEXT("SetTargetActorToNone");
	bNotifyTick = true;
}

void UBTTask_SetTargetActorToNone::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	
}

EBTNodeResult::Type UBTTask_SetTargetActorToNone::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AGAIController::TargetActorKey, nullptr);

	return Result = EBTNodeResult::Succeeded;
}
