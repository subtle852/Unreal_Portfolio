// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_IsInAttackRange.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/GAIController.h"
#include "Character/GCharacter.h"
#include "Character/GMonster.h"

UBTService_IsInAttackRange::UBTService_IsInAttackRange()
{
	NodeName = TEXT("IsInAttackRange");
	Interval = 1.f;
	AttackRange = 200.f;
}

void UBTService_IsInAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (AIController == nullptr || BlackboardComp == nullptr)
	{
		return;
	}

	AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	AGCharacter* TargetPlayerCharacter = Cast<AGCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AGAIController::TargetActorKey));

	if (Monster == nullptr || TargetPlayerCharacter == nullptr)
	{
		return;
	}

	bool bIsInAttackRange = IsInAttackRange(Monster, TargetPlayerCharacter);
	BlackboardComp->SetValueAsBool(AGAIController::IsInAttackRangeKey, bIsInAttackRange);
}

bool UBTService_IsInAttackRange::IsInAttackRange(AGMonster* Monster, AGCharacter* Target)
{
	if (Monster == nullptr || Target == nullptr)
	{
		return false;
	}

	float DistanceToTarget = FVector::Dist(Monster->GetActorLocation(), Target->GetActorLocation());
	return DistanceToTarget <= AttackRange;
}
