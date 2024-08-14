// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_MoveToBack.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GMonster.h"
#include "Controller/GAIController.h"

UBTTask_MoveToBack::UBTTask_MoveToBack()
{
	NodeName = "MoveToBackFromTarget";
	
	bNotifyTick = true;

	MoveToBackRange = 300.f;
}

void UBTTask_MoveToBack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	// ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));
	//
	// AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	// ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));
	//
	// if (Monster->bIsNowMovingToBackFromTarget == false)
	// {
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// }
}



EBTNodeResult::Type UBTTask_MoveToBack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//return Super::ExecuteTask(OwnerComp, NodeMemory);
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (AIController == nullptr || BlackboardComp == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	CachedOwnerComp = &OwnerComp;
	CachedAIController = AIController;
	
	//AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(AGAIController::TargetActorKey));
	AGCharacter* TargetActor = Cast<AGCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AGAIController::TargetActorKey));
	APawn* AIPawn = AIController->GetPawn();
	if (TargetActor == nullptr || AIPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	FVector ForwardVector = AIPawn->GetActorForwardVector();
	FVector MoveDirection = -ForwardVector;
	FVector Destination = AIPawn->GetActorLocation() + (MoveDirection * MoveToBackRange);
	
	FNavLocation NavLocation;
	if (NavSystem->ProjectPointToNavigation(Destination, NavLocation))
	{
		AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
		ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));
	
		//Monster->MoveToBackFromTarget(Direction);
		AIController->MoveToLocation(NavLocation.Location);
		if (AIController->OnAGAIController_MoveCompleted.IsAlreadyBound(this, &UBTTask_MoveToBack::OnMoveToBackCompleted) == false)
		{
			AIController->OnAGAIController_MoveCompleted.AddDynamic(this, &UBTTask_MoveToBack::OnMoveToBackCompleted);
		}
	
		Monster->bIsNowMovingToBackFromTarget = true;
		
		return EBTNodeResult::InProgress;
	}

	ensureMsgf(true, TEXT("Failed"));
	
	return EBTNodeResult::Failed;
}

void UBTTask_MoveToBack::OnMoveToBackCompleted()
{
	if(IsValid(CachedOwnerComp))
	{
		AGMonster* Monster = Cast<AGMonster>(CachedAIController->GetPawn());
		ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));
	
		Monster->bIsNowMovingToBackFromTarget = false;

		if (CachedAIController->OnAGAIController_MoveCompleted.IsAlreadyBound(this, &UBTTask_MoveToBack::OnMoveToBackCompleted) == true)
		{
			CachedAIController->OnAGAIController_MoveCompleted.RemoveDynamic(this, &UBTTask_MoveToBack::OnMoveToBackCompleted);
		}
		
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
}