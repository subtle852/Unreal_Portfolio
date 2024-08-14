// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_IsInModerateRange.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GMonster.h"
#include "Controller/GAIController.h"

const float UBTDecorator_IsInModerateRange::ModerateRange(400.f);

UBTDecorator_IsInModerateRange::UBTDecorator_IsInModerateRange()
{
	NodeName = TEXT("IsInModerateRange");
}

bool UBTDecorator_IsInModerateRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
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
		return Monster->GetDistanceTo(TargetPlayerCharacter) <= ModerateRange;
	}

	return false;
}
