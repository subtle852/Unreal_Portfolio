// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify/ANS_UpdateCanMove.h"
#include "Character/GPlayerCharacter.h"

void UANS_UpdateCanMove::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (IsValid(MeshComp) == true)
	{
		AGPlayerCharacter* AttackingCharacter = Cast<AGPlayerCharacter>(MeshComp->GetOwner());
		if (IsValid(AttackingCharacter) == true)
		{
			AttackingCharacter->OnCheckUpdateCanMove(true);
		}
	}
}

void UANS_UpdateCanMove::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UANS_UpdateCanMove::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp) == true)
	{
		AGPlayerCharacter* AttackingCharacter = Cast<AGPlayerCharacter>(MeshComp->GetOwner());
		if (IsValid(AttackingCharacter) == true)
		{
			AttackingCharacter->OnCheckUpdateCanMove(false);
		}
	}
}
