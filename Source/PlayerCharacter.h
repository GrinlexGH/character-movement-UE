// Copyright 1998 - 2018 Epic Games, Inc.All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PlayerCharacter.generated.h"

UCLASS(Blueprintable)
class OSCALE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

	/** Sensitivity of mouse */
	float Sensitivity = 0.6f;

	/** Speed of player */
	const float WalkSpeed = 200.0f;
	const float CrouchSpeed = 120.0f;
	const float RunSpeed = 400.0f;

	/** Does the player want to stand */
	bool bWantsToStandUp = false;
	bool bCrouched = false;
	bool bWantsToSprint = false;

	/** Arrays needed to check if the character can stand up */
	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> HitArray;

	/** Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComponent;

	/** CurveFloat that determines which pattern the player will crouch with */
	UPROPERTY(EditAnywhere, Category = Crouch)
		class UCurveFloat* CrouchCurveFloat;

	/** Timeline Component */
	UPROPERTY(VisibleAnywhere, Category = Crouch)
		class UTimelineComponent* TimelineComponent;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Movement functions */
	UFUNCTION()
		void MoveVertical(float Value);
	UFUNCTION()
		void MoveHorizontal(float Value);

	/** Jump functions */
	UFUNCTION()
		void StartJump();
	UFUNCTION()
		void StopJump();

	/** Camera movement functions */
	UFUNCTION()
		void PlayerCameraY(float Value);
	UFUNCTION()
		void PlayerCameraX(float Value);

	/** Crouch Functions */
	UFUNCTION()
		void CrouchFinished();
	UFUNCTION()
		void CrouchProgress(float Value);
	UFUNCTION()
		void StandUp();
	void PlayerCrouch();
	void PlayerUnCrouch();

	/** Sprint Functions */
	UFUNCTION()
		void Sprint();
	UFUNCTION()
		void UnSprint();
};