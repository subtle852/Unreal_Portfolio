// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GMonster.h"
#include "GBoss01.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API AGBoss01 : public AGMonster
{
	GENERATED_BODY()

public:
	AGBoss01();

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;

	virtual void DrawDetectLine(const bool bResult, FVector CenterPosition, float DetectRadius, FVector PCLocation, FVector MonsterLocation) override;
	
	UFUNCTION(NetMulticast, Unreliable)
	virtual void DrawDetectLine_NetMulticast(const bool bResult, FVector CenterPosition, float DetectRadius, FVector PCLocation, FVector MonsterLocation);
	
protected:
	virtual void OnCheckHit() override;
	
	virtual void BeginAttack() override;

	UFUNCTION(NetMulticast, Reliable)
	void PlayBasicAttackAnimMontage_NetMulticast();

	UFUNCTION(NetMulticast, Reliable)
	void DrawLine_NetMulticast(const bool bResult);

	virtual void EndAttack(class UAnimMontage* InMontage, bool bInterruped) override;

	virtual void Teleport() override;

	UFUNCTION(NetMulticast, Reliable)
	void Teleport_NetMulticast();
	
	virtual void TeleportEnd() override;

	UFUNCTION(NetMulticast, Reliable)
	void TeleportEnd_NetMulticast();

	virtual void MoveToBackFromTarget(const FVector& InDirection) override;
	
	UFUNCTION(Server, Reliable)
	void BeginMoveToBackFromTarget_Server(const FVector& InLocation);

	virtual void BeginShout() override;
	
	UFUNCTION(NetMulticast, Reliable)
	void PlayShoutAnimMontage_NetMulticast();
	
	virtual void EndShout(UAnimMontage* InMontage, bool bInterruped) override;

protected:
	// BodyMesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> ClothMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> BeardMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor01MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor02MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor03MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor04MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor05MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor06MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor07MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor08MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Body", meta = (AllowPrivateAccess))
	TObjectPtr<class USkeletalMeshComponent> Armor09MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Weapon", meta = (AllowPrivateAccess))
	TObjectPtr<class UStaticMeshComponent> WeaponMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Attack", meta = (AllowPrivateAccess))
	float BasicAttackRange = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Attack", meta = (AllowPrivateAccess))
	float BasicAttackRadius = 50.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Attack", meta = (AllowPrivateAccess))
	TObjectPtr<class UAnimMontage> Attack01Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Attack", meta = (AllowPrivateAccess))
	TObjectPtr<class UAnimMontage> Attack02Montage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Attack", meta = (AllowPrivateAccess))
	TObjectPtr<class UAnimMontage> Attack03Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Attack", meta = (AllowPrivateAccess))
	TObjectPtr<class UAnimMontage> ShoutMontage;

	// Weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Weapon", meta = (AllowPrivateAccess))
	TSubclassOf<class AGWeaponActor> WeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AGBoss01|Weapon", meta = (AllowPrivateAccess))
	TObjectPtr<class AGWeaponActor> WeaponInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Particle", meta = (AllowPrivateAccess))
	TObjectPtr<class UParticleSystem> TeleportBodyEffectEmitterTemplate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AGBoss01|Particle", meta = (AllowPrivateAccess))
	TObjectPtr<class UParticleSystemComponent> TeleportBodyEffectEmitterInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|Particle", meta = (AllowPrivateAccess))
	TObjectPtr<class UParticleSystem> TeleportTrailEffectEmitterTemplate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AGBoss01|Particle", meta = (AllowPrivateAccess))
	TObjectPtr<class UParticleSystemComponent> TeleportTrailEffectEmitterInstance;

	FTimerHandle TeleportEndDelayTimerHandle;

	float TeleportEndDelayThreshold = 0.5f;
	
};
