// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GProjectileActor.generated.h"

class UAnimInstance;
class UAnimMontage;
class UProjectileMovementComponent;

UCLASS()
class GAMEPROJECT_API AGProjectileActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGProjectileActor();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//USkeletalMeshComponent* GetMesh() const { return Mesh; }
	UStaticMeshComponent* GetMesh() const { return Mesh; }

	UProjectileMovementComponent* GetProjectileMovementComponent() { return ProjectileMovementComponent; }

	float GetLaunchSpeed() const { return LaunchSpeed; }

protected:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AGProjectileActor", meta = (AllowPrivateAccess))
	TObjectPtr<USceneComponent> Root;
	
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AGProjectileActor", meta = (AllowPrivateAccess))
	// TObjectPtr<USkeletalMeshComponent> Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGProjectileActor", meta = (AllowPrivateAccess))
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGProjectileActor", meta = (AllowPrivateAccess))
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGProjectileActor", meta = (AllowPrivateAccess))
	float LaunchSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGProjectileActor", meta = (AllowPrivateAccess))
	TObjectPtr<class UBoxComponent> BoxComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGProjectileActor", meta = (AllowPrivateAccess))
	TObjectPtr<class UParticleSystemComponent> ParticleSystemComponent;
};
