// Функции класса PlayerCharacter

#include "PlayerCharacter.h"


APlayerCharacter::APlayerCharacter()
{
	// Установка этого параметра для вызова Tick() в каждом кадре.
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("CrouchTimeline"));

	GetCharacterMovement()->CrouchedHalfHeight = 17.5f;
	GetCharacterMovement()->MaxStepHeight = 18.0f;
	GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 120.0f;
	GetCharacterMovement()->JumpZVelocity = 320.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	GetCharacterMovement()->MaxSwimSpeed = 290.0f;

	GetCapsuleComponent()->SetCapsuleHalfHeight(35.5f);
	GetCapsuleComponent()->SetCapsuleRadius(16.5f);

	CameraComponent->bUsePawnControlRotation = true;
	CameraComponent->SetRelativeLocation(FVector(0, 0, 32));
	
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat CrouchProgress;
	CrouchProgress.BindUFunction(this, FName("CrouchProgress"));
	FOnTimelineEventStatic CrouchFinished;
	CrouchFinished.BindUFunction(this, FName("CrouchFinished"));

	TimelineComponent->AddInterpFloat(CrouchCurveFloat, CrouchProgress);
	TimelineComponent->SetTimelineFinishedFunc(CrouchFinished);

}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (WantsToStandUp == true)
		StandUp();

	if (ShiftIsPressed == true)
	{
		if (GetCharacterMovement()->IsMovingOnGround() == true)
		{
			if (Crouched == false)
			{
				GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
			}
		}
	}

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
	float SensX = GetInputAxisValue("PlayerCameraX") * Sensivity;
	AddControllerYawInput(SensX);
}

void APlayerCharacter::PlayerCameraY(float Value)
{
	float SensY = GetInputAxisValue("PlayerCameraY") * Sensivity;
	AddControllerPitchInput(SensY);
}


void APlayerCharacter::MoveVertical(float Value)
{
	FVector Direction = APlayerCharacter::GetActorForwardVector();
	AddMovementInput(Direction, Value);
}

void APlayerCharacter::MoveHorizontal(float Value)
{
	FVector Direction = APlayerCharacter::GetActorRightVector();
	AddMovementInput(Direction, Value);
}


void APlayerCharacter::StartJump()
{
	bPressedJump = true;
}

void APlayerCharacter::StopJump()
{
	bPressedJump = false;
}


void APlayerCharacter::Sprint()
{
	ShiftIsPressed = true;
}

void APlayerCharacter::UnSprint()
{
	ShiftIsPressed = false;

	if (Crouched == false)
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}




void APlayerCharacter::PlayerCrouch()
{
	WantsToStandUp = false;
	Crouched = true;
	TimelineComponent->PlayFromStart();
}

void APlayerCharacter::PlayerUnCrouch()
{
	WantsToStandUp = true;
}

void APlayerCharacter::CrouchFinished()
{
	WantsToStandUp = false;
}

void APlayerCharacter::CrouchProgress(float Value)
{
	CameraComponent->SetRelativeLocation(FMath::Lerp(FVector(0.0f, 0.0f, 32.0f), FVector(0.0f, 0.0f, 11.0f), Value));
	GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(35.5f, 17.5f, Value));
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(WalkSpeed, CrouchSpeed, Value);
}

void APlayerCharacter::StandUp()
{
	bool Hit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), GetCapsuleComponent()->GetComponentLocation() + FVector(0, 0, 30),
																GetCapsuleComponent()->GetComponentLocation() +FVector(0, 0, 40), 10,
																UEngineTypes::ConvertToTraceType(ECC_Camera), false, ActorsToIgnore,
																EDrawDebugTrace::None, HitArray, true, FLinearColor::Red,
																FLinearColor::Green,
																5);
	if (Hit == false)
	{
		Crouched = false;
		TimelineComponent->Reverse();
	}
}