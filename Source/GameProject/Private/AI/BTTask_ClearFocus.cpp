// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ClearFocus.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GMonster.h"
#include "Character/GPlayerCharacter.h"
#include "Controller/GAIController.h"

UBTTask_ClearFocus::UBTTask_ClearFocus()
{
	NodeName = TEXT("ClearFocus");
}

EBTNodeResult::Type UBTTask_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));

	// AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	// checkf(IsValid(Monster) == true, TEXT("Invalid Monster"));
	
	//AIController->ClearFocus(EAIFocusPriority::Default);
	AIController->ClearFocus(EAIFocusPriority::Gameplay);
		
	return EBTNodeResult::Succeeded;
}
