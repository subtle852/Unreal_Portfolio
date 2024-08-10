// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FocusTarget.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GMonster.h"
#include "Character/GPlayerCharacter.h"
#include "Controller/GAIController.h"

UBTTask_FocusTarget::UBTTask_FocusTarget()
{
	NodeName = TEXT("FocusTarget");
}

EBTNodeResult::Type UBTTask_FocusTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));

	AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	checkf(IsValid(Monster) == true, TEXT("Invalid Monster"));
	
	if (AGPlayerCharacter* TargetPC = Cast<AGPlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AIController->TargetActorKey)))
	{
		AIController->SetFocus(TargetPC);
		
		return Result = EBTNodeResult::Succeeded;
	}

	return Result = EBTNodeResult::Failed;
}
