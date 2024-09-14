// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ShootAOE.h"
#include "Controller/GAIController.h"
#include "Character/GMonster.h"

UBTTask_ShootAOE::UBTTask_ShootAOE()
{
	NodeName = TEXT("ShootAOE");
	bNotifyTick = true;
}

void UBTTask_ShootAOE::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EBTNodeResult::Type UBTTask_ShootAOE::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));

	AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));

	CachedOwnerComp = &OwnerComp;
	CachedAIController = AIController;

	Monster->OnShootAOEMontageEndedDelegate_Task.BindUObject(this, &UBTTask_ShootAOE::EndShootAOE_Task);
	Monster->BeginShootAOE();

	return EBTNodeResult::InProgress;
}

void UBTTask_ShootAOE::EndShootAOE_Task(UAnimMontage* Montage, bool bInterrupted)
{
	AGMonster* Monster = Cast<AGMonster>(CachedAIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));
	
	if (Monster->OnShootAOEMontageEndedDelegate_Task.IsBound() == true)
	{
		Monster->OnShootAOEMontageEndedDelegate_Task.Unbind();
	}
	
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}


