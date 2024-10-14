// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GMonster.h"
#include "GOrc01.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API AGOrc01 : public AGMonster
{
	GENERATED_BODY()
	
public:
	AGOrc01();

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void DrawDetectLine(const bool bResult, FVector CenterPosition, float DetectRadius, FVector PCLocation, FVector MonsterLocation) override;
	
	UFUNCTION(NetMulticast, Unreliable)
	virtual void DrawDetectLine_NetMulticast(const bool bResult, FVector CenterPosition, float DetectRadius, FVector PCLocation, FVector MonsterLocation);
	
protected:
	virtual void OnCheckHit() override;
	
	virtual void BeginAttack() override;

	UFUNCTION(NetMulticast, Reliable)
	void PlayBasicAttackAnimMontage_NetMulticast(int32 InAttackRandNum);

	UFUNCTION(NetMulticast, Reliable)
	void DrawLine_NetMulticast(const bool bResult);

	virtual void EndAttack(class UAnimMontage* InMontage, bool bInterruped) override;
	
	virtual void BeginShout() override;
	
	UFUNCTION(NetMulticast, Reliable)
	void PlayShoutAnimMontage_NetMulticast();
	
	virtual void EndShout(UAnimMontage* InMontage, bool bInterruped) override;

	// HitReact
	//// Stun
	UFUNCTION(NetMulticast, Reliable)
	void PlayStunHitReactAnimMontage_NetMulticast();

	virtual void EndStunHitReact(class UAnimMontage* InMontage, bool bInterrupted);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|HitReact", meta = (AllowPrivateAccess))
	TObjectPtr<class UAnimMontage> StunHitReactMontage;

	//// KnockDown
	UFUNCTION(NetMulticast, Reliable)
	void PlayKnockDownHitReactAnimMontage_NetMulticast();

	virtual void EndKnockDownHitReact(class UAnimMontage* InMontage, bool bInterrupted);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|HitReact", meta = (AllowPrivateAccess))
	TObjectPtr<class UAnimMontage> KnockDownHitReactMontage;

	//// AirBound
	UFUNCTION(NetMulticast, Reliable)
	void PlayAirBoundHitReactAnimMontage_NetMulticast();

	virtual void EndAirBoundHitReact(class UAnimMontage* InMontage, bool bInterrupted);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|HitReact", meta = (AllowPrivateAccess))
	TObjectPtr<class UAnimMontage> AirBoundHitReactMontage;

	//// GroundBound
	UFUNCTION(NetMulticast, Reliable)
	void PlayGroundBoundHitReactAnimMontage_NetMulticast();

	virtual void EndGroundBoundHitReact(class UAnimMontage* InMontage, bool bInterrupted);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|HitReact", meta = (AllowPrivateAccess))
	TObjectPtr<class UAnimMontage> GroundBoundHitReactMontage;
	
	virtual void OnStartLying() override;// AN

	void ForceCall_EndMontageFunction(const uint8* InArr);

	void AdjustRotationToTarget();
	
protected:
	// Mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> ClothMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> BeardMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor01MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor02MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor03MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor04MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor05MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor06MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor07MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor08MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor09MeshComponent;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Weapon", meta = (AllowPrivateAccess))
	// TObjectPtr<class UStaticMeshComponent> WeaponMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Attack", meta = (AllowPrivateAccess))
	float BasicAttackRange = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Attack", meta = (AllowPrivateAccess))
	float BasicAttackRadius = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Attack", meta = (AllowPrivateAccess))
	TObjectPtr<class UAnimMontage> Attack01Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Attack", meta = (AllowPrivateAccess))
	TObjectPtr<class UAnimMontage> Attack02Montage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Attack", meta = (AllowPrivateAccess))
	TObjectPtr<class UAnimMontage> Attack03Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGOrc01|Attack", meta = (AllowPrivateAccess))
	TObjectPtr<class UAnimMontage> ShoutMontage;
	
	bool bWillHitReactDuplicate;
	
};
