// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GGliderActor.generated.h"

class UAnimInstance;
class UAnimMontage;

UCLASS()
class GAMEPROJECT_API AGGliderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGGliderActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Public function to get Mesh
	USkeletalMeshComponent* GetMesh() const { return Mesh; }

	TSubclassOf<UAnimInstance> GetUnarmedCharacterAnimLayer() const { return UnarmedCharacterAnimLayer; }

	//TSubclassOf<UAnimInstance> GetArmedCharacterAnimLayer() const { return ArmedCharacterAnimLayer; }

	//UAnimMontage* GetEquipAnimMontage() const { return EquipAnimMontage; }

	//UAnimMontage* GetUnequipAnimMontage() const { return UnequipAnimMontage; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AGGliderActor", meta = (AllowPrivateAccess))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGGliderActor|AnimLayer", meta = (AllowPrivateAccess))
	TSubclassOf<UAnimInstance> UnarmedCharacterAnimLayer;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGGliderActor|AnimLayer", meta = (AllowPrivateAccess))
	//TSubclassOf<UAnimInstance> ArmedCharacterAnimLayer;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGGliderActor|AnimMontage", meta = (AllowPrivateAccess))
	//TObjectPtr<UAnimMontage> EquipAnimMontage;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGGliderActor|AnimMontage", meta = (AllowPrivateAccess))
	//TObjectPtr<UAnimMontage> UnequipAnimMontage;

};

