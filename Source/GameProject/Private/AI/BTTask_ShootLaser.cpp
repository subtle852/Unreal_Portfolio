// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ShootLaser.h"
#include "Controller/GAIController.h"
#include "Character/GMonster.h"

UBTTask_ShootLaser::UBTTask_ShootLaser()
{
	NodeName = TEXT("ShootLaser");
	bNotifyTick = true;
}

void UBTTask_ShootLaser::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EBTNodeResult::Type UBTTask_ShootLaser::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));

	AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));

	CachedOwnerComp = &OwnerComp;
	CachedAIController = AIController;

	Monster->OnShootLaserFinishMontageEndedDelegate_Task.BindUObject(this, &UBTTask_ShootLaser::EndShootLaser_Task);
	Monster->BeginShootLaser();

	return EBTNodeResult::InProgress;
}

void UBTTask_ShootLaser::EndShootLaser_Task(UAnimMontage* Montage, bool bInterrupted)
{
	AGMonster* Monster = Cast<AGMonster>(CachedAIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));
	
	if (Monster->OnShootLaserFinishMontageEndedDelegate_Task.IsBound() == true)
	{
		Monster->OnShootLaserFinishMontageEndedDelegate_Task.Unbind();
	}
	
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}


