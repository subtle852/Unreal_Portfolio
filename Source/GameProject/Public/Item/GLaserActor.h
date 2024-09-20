// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GLaserActor.generated.h"

// DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLaserShrinkStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLaserShrinkEnd);

UCLASS()
class GAMEPROJECT_API AGLaserActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGLaserActor();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION(NetMulticast, Reliable)
	void ApplyRotation_NetMulticast(FRotator InRotator);

	UFUNCTION(NetMulticast, Reliable)
	void ApplyScale_NetMulticast(FVector InScale);

	UFUNCTION(NetMulticast, Reliable)
	void StartShrinking_NetMulticast();
	
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

public:
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AGTorusActor", meta = (AllowPrivateAccess))
	// FOnLaserShrinkStart OnLaserShrinkStart;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AGTorusActor", meta = (AllowPrivateAccess))
	FOnLaserShrinkEnd OnLaserShrinkEnd;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AGLaserActor", meta = (AllowPrivateAccess))
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGLaserActor", meta = (AllowPrivateAccess))
	TObjectPtr<class UBoxComponent> BoxComponent1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGLaserActor", meta = (AllowPrivateAccess))
	TObjectPtr<class UBoxComponent> BoxComponent2;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGLaserActor", meta = (AllowPrivateAccess))
	// TObjectPtr<class UBoxComponent> BoxComponent3;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGLaserActor", meta = (AllowPrivateAccess))
	// TObjectPtr<class UBoxComponent> BoxComponent4;
	
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGLaserActor", meta = (AllowPrivateAccess))
	// TObjectPtr<class UStaticMeshComponent> BodyStaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGLaserActor", meta = (AllowPrivateAccess))
	TObjectPtr<class UNiagaraComponent> NiagaraSystemComponent1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGLaserActor", meta = (AllowPrivateAccess))
	TObjectPtr<class UNiagaraComponent> NiagaraSystemComponent2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGLaserActor", meta = (AllowPrivateAccess))
	TObjectPtr<class UNiagaraComponent> NiagaraSystemComponent3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGLaserActor", meta = (AllowPrivateAccess))
	TObjectPtr<class UNiagaraComponent> NiagaraSystemComponent4;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AGLaserActor", meta = (AllowPrivateAccess))
	TObjectPtr<class URotatingMovementComponent> RotatingMovementComponent;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "AGLaserActor", meta = (AllowPrivateAccess))
	float RotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AGTorusActor", meta = (AllowPrivateAccess))
	uint8 bIsShrinking;

	FTimerHandle ShrinkTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AGTorusActor", meta = (AllowPrivateAccess))
	float ShrinkDelayTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AGTorusActor", meta = (AllowPrivateAccess))
	float ScaleUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AGTorusActor", meta = (AllowPrivateAccess))
	float ScaleDownRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AGTorusActor", meta = (AllowPrivateAccess))
	float MaxScale;
	
	float CurrentScale;
	
	float Lifetime;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGLaserActor", meta = (AllowPrivateAccess))
	float MaxLifetime;
	
	UPROPERTY(Replicated)
	TObjectPtr<AActor> OwnerActor;

};
