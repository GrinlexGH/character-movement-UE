// Все предопределения класса AFroggyPlayer

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


	float Sensivity = 0.5f;
	float WalkSpeed = 200.0f;
	float CrouchSpeed = 120.0f;
	float RunSpeed = 400.0f;

	bool WantsToStandUp = false;
	bool Crouched = false;
	bool ShiftIsPressed;

	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> HitArray;

	class UCameraComponent* CameraComponent;
	UPROPERTY(EditAnywhere, Category = "Timeline")
	class UTimelineComponent* TimelineComponent;
	UPROPERTY(EditAnywhere, Category = "Timeline")
	class UCurveFloat* CrouchCurveFloat;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void MoveVertical(float Value);
	UFUNCTION()
		void MoveHorizontal(float Value);

	UFUNCTION()
		void StartJump();
	UFUNCTION()
		void StopJump();

	UFUNCTION()
		void PlayerCameraY(float Value);
	UFUNCTION()
		void PlayerCameraX(float Value);

	UFUNCTION()
		void CrouchFinished();
	UFUNCTION()
		void CrouchProgress(float Value);
	UFUNCTION()
		void StandUp();
	void PlayerCrouch();
	void PlayerUnCrouch();

	UFUNCTION()
		void Sprint();
	UFUNCTION()
		void UnSprint();
	
};
