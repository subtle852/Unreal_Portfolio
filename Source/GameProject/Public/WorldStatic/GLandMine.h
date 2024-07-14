// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GLandMine.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UParticleSystemComponent;

UCLASS()
class GAMEPROJECT_API AGLandMine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGLandMine();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnLandMineBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void SpawnEffect_NetMulticast();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_IsExploded();
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGLandMine", meta = (AllowPrivateAccess))
	TObjectPtr<UBoxComponent> BodyBoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGLandMine", meta = (AllowPrivateAccess))
	TObjectPtr<UStaticMeshComponent> BodyStaticMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGLandMine", meta = (AllowPrivateAccess))
	TObjectPtr<UParticleSystemComponent> ParticleSystemComponent;

	UPROPERTY(ReplicatedUsing=OnRep_IsExploded, BlueprintReadOnly, Category = "AGLandMine", meta = (AllowPrivateAccess))
	uint8 bIsExploded : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGLandMine", meta = (AllowPrivateAccess))
	TObjectPtr<class UMaterial> ExplodedMaterial;
	
};

