#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GItemBox.generated.h"

UCLASS()
class GAMEPROJECT_API AGItemBox : public AActor
{
    GENERATED_BODY()

public:
    AGItemBox();

    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

    UFUNCTION()
    void OnEffectFinish(class UParticleSystemComponent* ParticleSystem);

private:
    UPROPERTY(VisibleAnywhere, Category = AGItemBox)
    TObjectPtr<class UBoxComponent> BoxComponent;

    UPROPERTY(EditAnywhere, Category = AGItemBox)
    TObjectPtr<class UStaticMeshComponent> StaticMeshComponent;

    UPROPERTY(EditAnywhere, Category = AGItemBox)
    TObjectPtr<class UParticleSystemComponent> ParticleSystemComponent;

};
