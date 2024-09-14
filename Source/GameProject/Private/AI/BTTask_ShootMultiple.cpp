// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ShootMultiple.h"
#include "Controller/GAIController.h"
#include "Character/GMonster.h"

UBTTask_ShootMultiple::UBTTask_ShootMultiple()
{
	NodeName = TEXT("ShootMultiple");
	bNotifyTick = true;
}

void UBTTask_ShootMultiple::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EBTNodeResult::Type UBTTask_ShootMultiple::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));

	AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));

	CachedOwnerComp = &OwnerComp;
	CachedAIController = AIController;

	Monster->OnShootMultipleMontageEndedDelegate_Task.BindUObject(this, &UBTTask_ShootMultiple::EndShootMultiple_Task);
	Monster->BeginShootMultiple();

	return EBTNodeResult::InProgress;
}

void UBTTask_ShootMultiple::EndShootMultiple_Task(UAnimMontage* Montage, bool bInterrupted)
{
	AGMonster* Monster = Cast<AGMonster>(CachedAIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));
	
	if (Monster->OnShootMultipleMontageEndedDelegate_Task.IsBound() == true)
	{
		Monster->OnShootMultipleMontageEndedDelegate_Task.Unbind();
	}
	
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}