// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ShootWind.h"
#include "Controller/GAIController.h"
#include "Character/GMonster.h"

UBTTask_ShootWind::UBTTask_ShootWind()
{
	NodeName = TEXT("ShootWind");
	bNotifyTick = true;
}

void UBTTask_ShootWind::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EBTNodeResult::Type UBTTask_ShootWind::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));

	AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));

	CachedOwnerComp = &OwnerComp;
	CachedAIController = AIController;

	Monster->OnShootWindMontageEndedDelegate_Task.BindUObject(this, &UBTTask_ShootWind::EndShootWind_Task);
	Monster->BeginShootWind();

	return EBTNodeResult::InProgress;
}

void UBTTask_ShootWind::EndShootWind_Task(UAnimMontage* Montage, bool bInterrupted)
{
	AGMonster* Monster = Cast<AGMonster>(CachedAIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));

	Monster->EndShootWind(Montage, bInterrupted);
	
	if (Monster->OnShootWindMontageEndedDelegate_Task.IsBound() == true)
	{
		Monster->OnShootWindMontageEndedDelegate_Task.Unbind();
	}
	
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}
