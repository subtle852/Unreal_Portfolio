// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_IsInModerateRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GCharacter.h"
#include "Character/GMonster.h"
#include "Controller/GAIController.h"
#include "GameFramework/Pawn.h"

UBTService_IsInModerateRange::UBTService_IsInModerateRange()
{
	NodeName = TEXT("IsInModerateRange");
	Interval = 1.f;
	ModerateRange = 400.f;
}

void UBTService_IsInModerateRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

	bool bIsInModerateRange = IsInModerateRange(Monster, TargetPlayerCharacter);
	BlackboardComp->SetValueAsBool(AGAIController::IsInModerateRangeKey, bIsInModerateRange);

}

bool UBTService_IsInModerateRange::IsInModerateRange(AGMonster* Monster, AGCharacter* Target)
{
	if (Monster == nullptr || Target == nullptr)
	{
		return false;
	}

	float DistanceToTarget = FVector::Dist(Monster->GetActorLocation(), Target->GetActorLocation());
	return DistanceToTarget <= ModerateRange;
}
