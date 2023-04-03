// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletActor.generated.h"

UCLASS()
class NETWORKPROJECT_API ABulletActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "bullet setting")
	class USphereComponent* sphereComp;

	UPROPERTY(EditDefaultsOnly, Category = "bullet setting")
	class UStaticMeshComponent* meshComp;

	UPROPERTY(EditAnywhere, Category = "bullet setting")
	float moveSpeed = 200.0f;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, Category = "bullet setting")
	class UParticleSystem* fireEffect;
	
	UPROPERTY(EditAnywhere)
	int32 attackPower;

	virtual void Destroyed();
	
};