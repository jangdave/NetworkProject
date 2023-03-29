// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "../NetworkProjectCharacter.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	player = Cast<ANetworkProjectCharacter>(GetOwningActor());
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(player == nullptr)
	{
		return;
	}

	moveSpeed = player->GetVelocity().Length();

#pragma region Calculate Angle

	//FVector forwarVec = player->GetActorForwardVector();
	// forward ���� ã�� ���
	//FVector forwarVec = FRotationMatrix(player->GetActorRotation()).GetUnitAxis(EAxis::X);
	//FVector normalVelocity = player->GetVelocity().GetSafeNormal2D();

	// ȸ�� ���� ���ϱ�
	//double cosTheta = FVector::DotProduct(forwarVec, normalVelocity);
	//double radian = FMath::Acos(cosTheta);
	//float degree = FMath::RadiansToDegrees(radian);

	//FVector rightVec = FRotationMatrix(player->GetActorRotation()).GetUnitAxis(EAxis::Y);

	// ȸ�� ���� ���
	//cosTheta = FVector::DotProduct(rightVec, normalVelocity);
	//rotAngle = cosTheta >= 0 ? degree : degree * -1.0f;

#pragma endregion
	// ���� ������ ����� �����ϰ� �Ʒ� �Լ��� �������

	//rotAngle = CalculateDirection(player->GetVelocity(), player->GetActorRotation());
	// ������ �α� ���� UKismetAnimationLibrary ����϶�� ���
	rotAngle = UKismetAnimationLibrary::CalculateDirection(player->GetVelocity(), player->GetActorRotation());

	bIsJumping = player->GetCharacterMovement()->IsFalling();

	FRotator viewRot = player->GetBaseAimRotation();
	FRotator playerRot = player->GetActorRotation();
	FRotator deltaRot = playerRot - viewRot;
	aimPitch = FMath::Clamp(deltaRot.GetNormalized().Pitch, -45.0f, 45.0f);
}
