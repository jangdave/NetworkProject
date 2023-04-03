// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "NetworkProjectCharacter.generated.h"


UCLASS(config=Game)
class ANetworkProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// 위젯 추가
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* playerInfoUI;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReleaseAction;

public:
	ANetworkProjectCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			
	void Fire();

	void ReleaseWeapon();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditDefaultsOnly, Category = "my settings")
	TSubclassOf<class ABulletActor> bulletFactory;

	UPROPERTY(EditDefaultsOnly, Category = "my settings")
	class UAnimMontage* FireMontage;

	UPROPERTY(EditDefaultsOnly, Category = "my settings")
	class UAnimMontage* HitMontage;

	UPROPERTY(EditDefaultsOnly, Category = "my settings")
	class UAnimMontage* NoAmmoMontage;

	UPROPERTY(EditDefaultsOnly, Category = "my settings")
	int32 maxHP = 100;

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "my settings")
	int32 curHP;

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "my settings")
	int32 ammo;

	UPROPERTY(VisibleDefaultsOnly, Category = "my settings")
	class UPlayerInfoWidget* infoWidget;

	UPROPERTY()
	class AWeaponActor* owningWeapon;

	// 동기화
	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerFire();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastFire(bool bHasAmmo);

	UFUNCTION(Client, Unreliable)
	void ClientFire(int32 damage);

	UFUNCTION()
	void SetHealth(int32 value);

	UFUNCTION()
	void AddHealth(int32 value);

	UFUNCTION(Server, Unreliable)
	void ServerDamageProcess(int32 value);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastDamageProcess();

	UFUNCTION(Server, Unreliable)
	void SetServerName(const FString& name);

	FORCEINLINE int32 GetHealth() { return curHP; };

	FORCEINLINE int32 GetAmmo() { return ammo; };

	FORCEINLINE bool IsDead() { return bIsDead; };

	void SpawnBullet();

	class ABulletActor* bullet;

	class UPlayerAnimInstance* anim;
	
private:
	FString PrintInfo();

	int32 number;

	UPROPERTY(Replicated)
	int32 repNumber;

	UPROPERTY(Replicated)
	FString myName;

	UPROPERTY(Replicated)
	bool bFireDelay;

	UPROPERTY()
	bool bIsDead;

	class UServerGameInstance* gameInstance;
};