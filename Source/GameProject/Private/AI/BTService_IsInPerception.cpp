// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_IsInPerception.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/GAIController.h"
#include "Character/GCharacter.h"
#include "Character/GMonster.h"
#include "Component/GStatComponent.h"

UBTService_IsInPerception::UBTService_IsInPerception()
{
}

void UBTService_IsInPerception::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (AIController == nullptr || BlackboardComp == nullptr)
	{
		return;
	}

	AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	AGCharacter* TargetPlayerCharacter = Cast<AGCharacter>(
		OwnerComp.GetBlackboardComponent()->GetValueAsObject(AGAIController::TargetActorKey));

	if (Monster == nullptr || TargetPlayerCharacter == nullptr)
	{
		return;
	}

	if (TargetPlayerCharacter->GetStatComponent()->GetCurrentHP() <= 0.0f)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AGAIController::TargetActorKey, nullptr);
	}
}
