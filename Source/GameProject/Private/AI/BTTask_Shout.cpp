// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Shout.h"

#include "Character/GMonster.h"
#include "Controller/GAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Shout::UBTTask_Shout()
{
	NodeName = TEXT("Shout");
	bNotifyTick = false;
}

void UBTTask_Shout::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);


}

EBTNodeResult::Type UBTTask_Shout::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));

	AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));

	CachedOwnerComp = &OwnerComp;
	CachedAIController = AIController;

	// if(Monster->bIsShout == true)
	// {
	// 	// 이미 Shout한 경우
	// 	return EBTNodeResult::Succeeded;
	// }
	
	Monster->OnShoutMontageEndedDelegate_Task.BindUObject(this, &ThisClass::EndShout_Task);
	Monster->BeginShout();

	return EBTNodeResult::InProgress;
}

void UBTTask_Shout::EndShout_Task(UAnimMontage* Montage, bool bInterrupted)
{
	AGMonster* Monster = Cast<AGMonster>(CachedAIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));

	CachedOwnerComp->GetBlackboardComponent()->SetValueAsBool(AGAIController::IsShoutKey, true);
	
	if (Monster->OnShoutMontageEndedDelegate_Task.IsBound() == true)
	{
		Monster->OnShoutMontageEndedDelegate_Task.Unbind();
	}
	
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}