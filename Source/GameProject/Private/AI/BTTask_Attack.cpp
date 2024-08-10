// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Attack.h"
#include "Controller/GAIController.h"
#include "Character/GMonster.h"

UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));

	AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));
	
	// if (Monster->bIsNowAttacking == false)
	// {
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// }
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));

	AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));

	CachedOwnerComp = &OwnerComp;
	CachedAIController = AIController;

	Monster->OnBasicAttackMontageEndedDelegate_Task.BindUObject(this, &UBTTask_Attack::EndAttack_Task);
	Monster->BeginAttack();

	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::EndAttack_Task(UAnimMontage* Montage, bool bInterrupted)
{
	AGMonster* Monster = Cast<AGMonster>(CachedAIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));
	
	if (Monster->OnBasicAttackMontageEndedDelegate_Task.IsBound() == true)
	{
		Monster->OnBasicAttackMontageEndedDelegate_Task.Unbind();
	}
	
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}