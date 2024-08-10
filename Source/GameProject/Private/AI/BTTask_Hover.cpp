// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Hover.h"

#include "NavigationSystem.h"
#include "Character/GMonster.h"
#include "Controller/GAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_Hover::UBTTask_Hover()
{
	NodeName = "Hover";
}

void UBTTask_Hover::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	// ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));
	//
	// AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	// ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));
	//
	// if (Monster->bIsNowAttacking == false)
	// {
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// }
}

EBTNodeResult::Type UBTTask_Hover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	CachedOwnerComp = &OwnerComp;

	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (AIController == nullptr || BlackboardComp == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	CachedAIController = AIController;
	
	APawn* AIPawn = AIController->GetPawn();
	if (AIPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// Choose a random direction (left or right) and calculate the destination
	float MoveDistance = 300.0f; // You can adjust the distance AI moves
	FVector Destination = ChooseRandomDirection(AIPawn, MoveDistance);
	
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FNavLocation NavLocation;
	if (NavSystem->ProjectPointToNavigation(Destination, NavLocation))
	{
		AIController->MoveToLocation(NavLocation.Location);
		
		//AIController->ReceiveMoveCompleted.AddDynamic(this, &UBTTask_Hover::OnHoverCompleted);
		//HoverCompletedDelegateHandle = AIController->ReceiveMoveCompleted.AddUObject(this, &UBTTask_Hover::OnHoverCompleted);
		AIController->OnAGAIController_MoveCompleted.AddDynamic(this, &UBTTask_Hover::OnHoverCompleted);
		
		AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
		ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));
	
		Monster->bIsNowHovering = true;
		//Monster->GetCharacterMovement()->bOrientRotationToMovement = false;
		//Monster->GetCharacterMovement()->bUseControllerDesiredRotation = true;

		
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::InProgress;
}

FVector UBTTask_Hover::ChooseRandomDirection(AActor* AIActor, float Distance)
{
	float RandomDirection = FMath::RandBool() ? 1.0f : -1.0f;
	
	FVector RightVector = AIActor->GetActorRightVector();
	FVector MoveDirection = RightVector * RandomDirection;
	FVector Destination = AIActor->GetActorLocation() + (MoveDirection * Distance);

	return Destination;
}

void UBTTask_Hover::OnHoverCompleted()
{
	if(IsValid(CachedOwnerComp))
	{
		AGMonster* Monster = Cast<AGMonster>(CachedAIController->GetPawn());
		ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));
	
		Monster->bIsNowHovering = false;
		
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
}
