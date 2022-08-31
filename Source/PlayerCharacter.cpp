// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#include "PlayerCharacter.h"

APlayerCharacter::APlayerCharacter()
{
	//Setting this option to call Tick() on every frame.
	PrimaryActorTick.bCanEverTick = true;

	//Camera component initialization
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//Timeline component initialization
	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("CrouchTimeline"));



	//Player options initialization (like in source engine)
	GetCharacterMovement()->CrouchedHalfHeight = 17.5f;
	GetCharacterMovement()->MaxStepHeight = 18.0f;
	GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 120.0f;
	GetCharacterMovement()->JumpZVelocity = 320.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	GetCharacterMovement()->MaxSwimSpeed = 290.0f;
	GetCapsuleComponent()->SetCapsuleHalfHeight(35.5f);
	GetCapsuleComponent()->SetCapsuleRadius(16.5f);

	//To make the camera follow the controller
	CameraComponent->bUsePawnControlRotation = true;
	CameraComponent->SetRelativeLocation(FVector(0, 0, 32));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Initialization of functions that will be used by the Timeline component
	FOnTimelineFloat CrouchProgress;
	CrouchProgress.BindUFunction(this, FName("CrouchProgress"));
	FOnTimelineEventStatic CrouchFinished;
	CrouchFinished.BindUFunction(this, FName("CrouchFinished"));

	TimelineComponent->AddInterpFloat(CrouchCurveFloat, CrouchProgress);
	TimelineComponent->SetTimelineFinishedFunc(CrouchFinished);

	//FOV initialization
	CameraComponent->FieldOfView = 110;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWantsToStandUp)
		StandUp();

	//Player can run only if he on ground and he not crouching
	if (bWantsToSprint && GetCharacterMovement()->IsMovingOnGround() && !bCrouched)
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("PlayerMoveVertical", this, &APlayerCharacter::MoveVertical);
	PlayerInputComponent->BindAxis("PlayerMoveHorizontal", this, &APlayerCharacter::MoveHorizontal);

	PlayerInputComponent->BindAxis("PlayerCameraX", this, &APlayerCharacter::PlayerCameraX);
	PlayerInputComponent->BindAxis("PlayerCameraY", this, &APlayerCharacter::PlayerCameraY);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::StopJump);

	PlayerInputComponent->BindAction("CrouchPressed", IE_Pressed, this, &APlayerCharacter::PlayerCrouch);
	PlayerInputComponent->BindAction("CrouchPressed", IE_Released, this, &APlayerCharacter::PlayerUnCrouch);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::UnSprint);
}


void APlayerCharacter::PlayerCameraX(float Value)
{
	const float SensX = GetInputAxisValue("PlayerCameraX") * Sensitivity;
	AddControllerYawInput(SensX);
}

void APlayerCharacter::PlayerCameraY(float Value)
{
	const float SensY = GetInputAxisValue("PlayerCameraY") * Sensitivity;
	AddControllerPitchInput(SensY);
}


void APlayerCharacter::MoveVertical(float Value)
{
	const FVector Direction = APlayerCharacter::GetActorForwardVector();
	AddMovementInput(Direction, Value);
}

void APlayerCharacter::MoveHorizontal(float Value)
{
	const FVector Direction = APlayerCharacter::GetActorRightVector();
	AddMovementInput(Direction, Value);
}



void APlayerCharacter::StartJump() { bPressedJump = true; }
void APlayerCharacter::StopJump() { bPressedJump = false; }

void APlayerCharacter::Sprint() { bWantsToSprint = true; }
void APlayerCharacter::UnSprint()
{
	bWantsToSprint = false;

	if (bCrouched == false)
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}



void APlayerCharacter::PlayerCrouch()
{
	bWantsToStandUp = false;
	bCrouched = true;
	TimelineComponent->PlayFromStart();
}

void APlayerCharacter::PlayerUnCrouch() { bWantsToStandUp = true; }
void APlayerCharacter::CrouchFinished() { bWantsToStandUp = false; }

void APlayerCharacter::CrouchProgress(float Value)
{
	CameraComponent->SetRelativeLocation(FMath::Lerp(FVector(0.0f, 0.0f, 32.0f), FVector(0.0f, 0.0f, 11.0f), Value));
	GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(35.5f, 17.5f, Value));
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(WalkSpeed, CrouchSpeed, Value);
}

void APlayerCharacter::StandUp()
{
	bool Hit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), GetCapsuleComponent()->GetComponentLocation() + FVector(0, 0, 30),
		GetCapsuleComponent()->GetComponentLocation() + FVector(0, 0, 40), 10,
		UEngineTypes::ConvertToTraceType(ECC_Camera), false, ActorsToIgnore,
		EDrawDebugTrace::None, HitArray, true, FLinearColor::Red,
		FLinearColor::Green,
		5);

	if (Hit == false)
	{
		bCrouched = false;
		TimelineComponent->Reverse();
	}
}