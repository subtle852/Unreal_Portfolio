// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GTorusActor.generated.h"

UCLASS()
class GAMEPROJECT_API AGTorusActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGTorusActor();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION(NetMulticast, Reliable)
	void ApplyScale_NetMulticast(FVector InScale);
	
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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AGTorusActor", meta = (AllowPrivateAccess))
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGTorusActor", meta = (AllowPrivateAccess))
	TObjectPtr<class UStaticMeshComponent> BodyStaticMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AGTorusActor", meta = (AllowPrivateAccess))
	float GrowthRate = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AGTorusActor", meta = (AllowPrivateAccess))
	float MaxScale = 50.0f;
	
	float CurrentScale = 1.0f;
	
	float Lifetime;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGTorusActor", meta = (AllowPrivateAccess))
	float MaxLifetime;
	
	UPROPERTY(Replicated)
	TObjectPtr<AActor> OwnerActor;
};
