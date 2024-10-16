// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GWindProjectileActor.generated.h"

class UAnimInstance;
class UAnimMontage;
class UProjectileMovementComponent;
class UNiagaraComponent;

UCLASS()
class GAMEPROJECT_API AGWindProjectileActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGWindProjectileActor();

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
	void OnEffectFinish(class UNiagaraComponent* NiagaraComponentSystem);

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
	TObjectPtr<UNiagaraComponent> NiagaraSystemComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGProjectileActor", meta = (AllowPrivateAccess))
	TObjectPtr<UNiagaraComponent> ExplodeNiagaraSystemComponent;
	
	UPROPERTY(Replicated)
	TObjectPtr<AActor> OwnerActor;
};
