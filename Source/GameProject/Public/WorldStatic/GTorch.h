// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GTorch.generated.h"

UCLASS()
class GAMEPROJECT_API AGTorch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGTorch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GSTorch", meta = (AllowPrivateAccess))
	TObjectPtr<class UBoxComponent> BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GSTorch", meta = (AllowPrivateAccess))
	TObjectPtr<class UStaticMeshComponent> BodyStaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GSTorch", meta = (AllowPrivateAccess))
	TObjectPtr<class UPointLightComponent> PointLightComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GSTorch", meta = (AllowPrivateAccess))
	TObjectPtr<class UParticleSystemComponent> ParticleSystemComponent;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "GSTorch", meta = (AllowPrivateAccess))
	int32 ID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GSTorch", meta = (AllowPrivateAccess))
	float RotationSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GSTorch", meta = (AllowPrivateAccess))
	TObjectPtr<class URotatingMovementComponent> RotatingMovementComponent;

};

