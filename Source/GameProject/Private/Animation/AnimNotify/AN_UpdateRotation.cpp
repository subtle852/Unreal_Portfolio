// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify/AN_UpdateRotation.h"
#include "Character/GPlayerCharacter.h"

void UAN_UpdateRotation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (IsValid(MeshComp) == true)
	{
		AGPlayerCharacter* AttackingCharacter = Cast<AGPlayerCharacter>(MeshComp->GetOwner());
		if (IsValid(AttackingCharacter) == true)
		{
			AttackingCharacter->OnCheckUpdateRotation();
		}

		// AGMonster* AttackingMonster = Cast<AGMonster>(MeshComp->GetOwner());
		// if (IsValid(AttackingMonster) == true)
		// {
		// 	AttackingMonster->OnCheckHit();
		// }
	}
}
