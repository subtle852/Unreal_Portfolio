// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_IsInPerceptionRange.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GMonster.h"
#include "Controller/GAIController.h"

const float UBTDecorator_IsInPerceptionRange::PerceptionRange(700.f);

UBTDecorator_IsInPerceptionRange::UBTDecorator_IsInPerceptionRange()
{
	NodeName = TEXT("IsInPerceptionRange");
}

bool UBTDecorator_IsInPerceptionRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	ensureMsgf(bResult != false, TEXT("Super::CalculateRawConditionValue() function has returned false"));

	AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
	ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));

	AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
	ensureMsgf(IsValid(Monster), TEXT("Invalid Monster"));
	
	AGCharacter* TargetPlayerCharacter = Cast<AGCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AGAIController::TargetActorKey));
	if (IsValid(TargetPlayerCharacter) && TargetPlayerCharacter->IsPlayerControlled() == true)
	{
		return Monster->GetDistanceTo(TargetPlayerCharacter) <= PerceptionRange;
	}

	return false;
}
