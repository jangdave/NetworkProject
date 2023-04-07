// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Setting")
	float rotAngle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Setting")
	float moveSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Setting")
	bool bIsJumping = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Setting")
	float aimPitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Setting")
	bool bIsDead = false;

	UFUNCTION(BlueprintCallable)
	void AnimNotify_WalkSound(class USoundBase* source, USoundAttenuation* attenuation);

private:
	class ANetworkProjectCharacter* player;
};
