// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_JumpAttack.h"
#include "Controller/GAIController.h"
#include "Character/GMonster.h"

UBTTask_JumpAttack::UBTTask_JumpAttack()
{
	NodeName = TEXT("JumpAttack");
	bNotifyTick = true;
}

void UBTTask_JumpAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EBTNodeResult::Type UBTTask_JumpAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));

	AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));

	CachedOwnerComp = &OwnerComp;
	CachedAIController = AIController;

	Monster->OnJumpAttackMontageEndedDelegate_Task.BindUObject(this, &UBTTask_JumpAttack::EndJumpAttack_Task);
	Monster->BeginJumpAttack();

	return EBTNodeResult::InProgress;
}

void UBTTask_JumpAttack::EndJumpAttack_Task(UAnimMontage* Montage, bool bInterrupted)
{
	AGMonster* Monster = Cast<AGMonster>(CachedAIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));
	
	if (Monster->OnJumpAttackMontageEndedDelegate_Task.IsBound() == true)
	{
		Monster->OnJumpAttackMontageEndedDelegate_Task.Unbind();
	}
	
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}