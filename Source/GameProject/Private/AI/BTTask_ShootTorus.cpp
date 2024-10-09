// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ShootTorus.h"
#include "Controller/GAIController.h"
#include "Character/GMonster.h"

UBTTask_ShootTorus::UBTTask_ShootTorus()
{
	NodeName = TEXT("ShootTorus");
	bNotifyTick = true;
}

void UBTTask_ShootTorus::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EBTNodeResult::Type UBTTask_ShootTorus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));

	AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));

	CachedOwnerComp = &OwnerComp;
	CachedAIController = AIController;

	Monster->OnShootTorusMontageEndedDelegate_Task.BindUObject(this, &UBTTask_ShootTorus::EndShootTorus_Task);
	Monster->BeginShootTorus();

	return EBTNodeResult::InProgress;
}

void UBTTask_ShootTorus::EndShootTorus_Task(UAnimMontage* Montage, bool bInterrupted)
{
	AGMonster* Monster = Cast<AGMonster>(CachedAIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));

	Monster->EndShootTorus(Montage, bInterrupted);
	
	if (Monster->OnShootTorusMontageEndedDelegate_Task.IsBound() == true)
	{
		Monster->OnShootTorusMontageEndedDelegate_Task.Unbind();
	}
	
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}