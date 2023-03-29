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
	// forward 백터 찾는 방법
	//FVector forwarVec = FRotationMatrix(player->GetActorRotation()).GetUnitAxis(EAxis::X);
	//FVector normalVelocity = player->GetVelocity().GetSafeNormal2D();

	// 회전 각도 구하기
	//double cosTheta = FVector::DotProduct(forwarVec, normalVelocity);
	//double radian = FMath::Acos(cosTheta);
	//float degree = FMath::RadiansToDegrees(radian);

	//FVector rightVec = FRotationMatrix(player->GetActorRotation()).GetUnitAxis(EAxis::Y);

	// 회전 방향 계산
	//cosTheta = FVector::DotProduct(rightVec, normalVelocity);
	//rotAngle = cosTheta >= 0 ? degree : degree * -1.0f;

#pragma endregion
	// 위의 공식을 충분히 숙지하고 아래 함수를 사용하자

	//rotAngle = CalculateDirection(player->GetVelocity(), player->GetActorRotation());
	// 컴파일 로그 보면 UKismetAnimationLibrary 사용하라고 뜬다
	rotAngle = UKismetAnimationLibrary::CalculateDirection(player->GetVelocity(), player->GetActorRotation());

	bIsJumping = player->GetCharacterMovement()->IsFalling();

	FRotator viewRot = player->GetBaseAimRotation();
	FRotator playerRot = player->GetActorRotation();
	FRotator deltaRot = playerRot - viewRot;
	aimPitch = FMath::Clamp(deltaRot.GetNormalized().Pitch, -45.0f, 45.0f);
}
