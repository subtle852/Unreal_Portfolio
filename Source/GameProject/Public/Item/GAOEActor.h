// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GAOEActor.generated.h"

UCLASS()
class GAMEPROJECT_API AGAOEActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGAOEActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	void TriggerExplosion();
	
	void HandleExplosion();

	UFUNCTION(NetMulticast, Reliable)
	void DeActivateCircleParticle_NetMulticast();

	UFUNCTION(NetMulticast, Reliable)
	void ActivateExplodeParticle_NetMulticast();

	UFUNCTION(NetMulticast, Reliable)
	void DrawDebug_NetMulticast(bool bInResult, FVector InCenterPosition);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGAOEActor", meta = (AllowPrivateAccess))
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGAOEActor", meta = (AllowPrivateAccess))
	TObjectPtr<UParticleSystemComponent> CircleParticle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGAOEActor", meta = (AllowPrivateAccess))
	TObjectPtr<UParticleSystemComponent> ExplodeParticle;

	FTimerHandle DelayTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGAOEActor", meta = (AllowPrivateAccess))
	float DelayTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGAOEActor", meta = (AllowPrivateAccess))
	float AOEExplosionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGAOEActor", meta = (AllowPrivateAccess))
	float AOEDamageAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGAOEActor", meta = (AllowPrivateAccess))
	float DestroyTime;
	
};
