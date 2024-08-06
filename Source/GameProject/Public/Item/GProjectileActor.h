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
	
	void InitializeHoming(AActor* Target);

protected:
	// UFUNCTION(Server, Reliable)
	// void InitializeHoming_Server(AActor* Target);
	//
	// UFUNCTION(NetMulticast, Reliable)
	// void InitializeHoming_NetMulticast(AActor* Target);
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(Server, Reliable)
	void OnHit_Server(FVector InNewLocation);

	UFUNCTION(NetMulticast, Reliable)
	void OnHit_NetMulticast(FVector InNewLocation);
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	  const FHitResult& SweepResult);

	UFUNCTION(Server, Reliable)
	void OnBeginOverlap_Server();
	
	UFUNCTION(NetMulticast, Reliable)
	void OnBeginOverlap_NetMulticast();

	UFUNCTION()
	void OnEffectFinish(class UParticleSystemComponent* ParticleSystem);

	UFUNCTION(Server, Reliable)
	void OnEffectFinish_Server();

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
	
	float Lifetime;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGProjectileActor", meta = (AllowPrivateAccess))
	float MaxLifetime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGProjectileActor", meta = (AllowPrivateAccess))
	float MaxDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGProjectileActor", meta = (AllowPrivateAccess))
	TObjectPtr<UParticleSystemComponent> ParticleSystemComponent;
	
	UPROPERTY(Replicated)
	TObjectPtr<AActor> OwnerActor;

	// UPROPERTY(Replicated)
	// TObjectPtr<AActor> HomingTarget;
	
};
