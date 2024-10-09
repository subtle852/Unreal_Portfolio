// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Shoot.h"
#include "Controller/GAIController.h"
#include "Character/GMonster.h"

UBTTask_Shoot::UBTTask_Shoot()
{
	NodeName = TEXT("Shoot");
	bNotifyTick = true;
}

void UBTTask_Shoot::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));

	AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));

	CachedOwnerComp = &OwnerComp;
	CachedAIController = AIController;

	Monster->OnShootMontageEndedDelegate_Task.BindUObject(this, &UBTTask_Shoot::EndShoot_Task);
	Monster->BeginShoot();

	return EBTNodeResult::InProgress;
}

void UBTTask_Shoot::EndShoot_Task(UAnimMontage* Montage, bool bInterrupted)
{
	AGMonster* Monster = Cast<AGMonster>(CachedAIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));

	Monster->EndShoot(Montage, bInterrupted);
	
	if (Monster->OnShootMontageEndedDelegate_Task.IsBound() == true)
	{
		Monster->OnShootMontageEndedDelegate_Task.Unbind();
	}
	
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}