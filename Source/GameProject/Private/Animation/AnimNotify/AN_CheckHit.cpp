// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify/AN_CheckHit.h"
#include "Character/GPlayerCharacter.h"
#include "Character/GMonster.h"

void UAN_CheckHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp) == true)
	{
		AGPlayerCharacter* AttackingCharacter = Cast<AGPlayerCharacter>(MeshComp->GetOwner());
		if (IsValid(AttackingCharacter) == true)
		{
			AttackingCharacter->OnCheckHit();
		}

		AGMonster* AttackingMonster = Cast<AGMonster>(MeshComp->GetOwner());
		if (IsValid(AttackingMonster) == true)
		{
			AttackingMonster->OnCheckHit();
		}
	}
}
