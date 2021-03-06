// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Orbit3DPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Engine/Engine.h"

AOrbit3DPawn::AOrbit3DPawn()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Flying/Meshes/UFO.UFO"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create static mesh component
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	PlaneMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());	// Set static mesh
	RootComponent = PlaneMesh;

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(RootComponent);	// Attach SpringArm to RootComponent
	SpringArm->TargetArmLength = 160.0f; // The camera follows at this distance behind the character
	SpringArm->SocketOffset = FVector(0.f,0.f,60.f);
	SpringArm->bEnableCameraLag = true;	// Allow camera to lag, higher speed = greater distance between camera and ship
	SpringArm->CameraLagSpeed = 25.f;
	SpringArm->bEnableCameraRotationLag = true;	// Camera rotation lag
	SpringArm->CameraRotationLagSpeed = 5.f;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);	// Attach the camera
	Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

	// Set handling parameters
	Acceleration = 1000.f;
	TurnSpeed = 50.f;
	MaxSpeed = 4000.f;
	MinSpeed = 500.f;
	CurrentForwardSpeed = 500.f;
	Velocity = FVector(0.f);
}

void AOrbit3DPawn::Tick(float DeltaSeconds)
{
	//UE_LOG(LogTemp, Warning, TEXT("AOrbit3DPawn DeltaSeconds = %f"), DeltaSeconds);


	// Calculate change in rotation this frame
	FRotator DeltaRotation(0,0,0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(DeltaRotation);


	FVector ThrustMove = FVector(CurrentForwardSpeed * DeltaSeconds, 0.f, 0.f);
	// Move plan forwards (with sweep so we stop when we collide with things)
	//AddActorLocalOffset(ThrustMove, true);
	AddActorWorldOffset(Velocity * DeltaSeconds + GetActorRotation().RotateVector(ThrustMove), true);
	//AddActorWorldOffset(Velocity, true);

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Vel = %s"), *Velocity.ToString()));

	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
}

void AOrbit3DPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Deflect along the surface when we collide.
	FRotator CurrentRotation = GetActorRotation();
	SetActorRotation(FQuat::Slerp(CurrentRotation.Quaternion(), HitNormal.ToOrientationQuat(), 0.025f));
}

void AOrbit3DPawn::IncrementVelocity(FVector Vec)
{
	Velocity += Vec;
}


void AOrbit3DPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // Check if PlayerInputComponent is valid (not NULL)
	check(PlayerInputComponent);

	// Bind our control axis' to callback functions
	PlayerInputComponent->BindAxis("Thrust", this, &AOrbit3DPawn::ThrustInput);
	PlayerInputComponent->BindAxis("MoveUp", this, &AOrbit3DPawn::MoveUpInput);
	PlayerInputComponent->BindAxis("MoveRight", this, &AOrbit3DPawn::MoveRightInput);
	PlayerInputComponent->BindAxis("RollRight", this, &AOrbit3DPawn::RollRightInput);
	PlayerInputComponent->BindAxis("ControlManualCameraX", this, &AOrbit3DPawn::ControlManualCameraInputX);
	PlayerInputComponent->BindAxis("ControlManualCameraY", this, &AOrbit3DPawn::ControlManualCameraInputY);

	// Bind actions
	PlayerInputComponent->BindAction("EnableManualCamera", IE_Pressed, this, &AOrbit3DPawn::EnableManualCameraInput);
	PlayerInputComponent->BindAction("EnableManualCamera", IE_Released, this, &AOrbit3DPawn::DisableManualCameraInput);
}

void AOrbit3DPawn::ThrustInput(float Val)
{
	// Is there any input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	// If input is not held down, reduce speed
	float CurrentAcc = bHasInput ? (Val * Acceleration) : 0.f;// (-0.5f * Acceleration);
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
}

void AOrbit3DPawn::MoveUpInput(float Val)
{
	// Target pitch speed is based in input
	float TargetPitchSpeed = (Val * TurnSpeed * -1.f);

	// When steering, we decrease pitch slightly
	TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

	// Smoothly interpolate to target pitch speed
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AOrbit3DPawn::RollRightInput(float Val)
{
	// Target yaw speed is based on input
	float TargetRollSpeed = (Val * TurnSpeed * 2.f);

	// Smoothly interpolate roll speed
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AOrbit3DPawn::MoveRightInput(float Val)
{
	// Target yaw speed is based on input
	float TargetYawSpeed = (Val * TurnSpeed);

	// Smoothly interpolate to target yaw speed
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	// Is there any left/right input?
	const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// If turning, yaw value is used to influence roll
	// If not turning, roll to reverse current roll value.
	float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.5f) : 0.f;// (GetActorRotation().Roll * -2.f);

	// Smoothly interpolate roll speed
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AOrbit3DPawn::EnableManualCameraInput()
{
	bIsManualCamera = true;
	// Disable rotation lag --> Want controlling the camera to feel responsive
	SpringArm->bEnableCameraRotationLag = false;
}

void AOrbit3DPawn::DisableManualCameraInput()
{
	bIsManualCamera = false;
	// Enable rotation lag
	SpringArm->bEnableCameraRotationLag = true;
	// Reset rotation
	// Since we have rotation lag, the spring arm smoothly moves back to its original position
	SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
}

void AOrbit3DPawn::ControlManualCameraInputX(float Val)
{
	// Only adjust when the player is manually controlling the camera (LMB is held down)
	if (bIsManualCamera)
	{
		// Adjust the arm's yaw based on the input
		// Rotate the spring arm so the camera rotates about the spaceship
		SpringArm->AddRelativeRotation(FRotator(0.f, Val, 0.f));
	}
}

void AOrbit3DPawn::ControlManualCameraInputY(float Val)
{
	if (bIsManualCamera)
	{
		// Adjust the arm's pitch, clamped between -85 and 85 degrees
		FRotator NewRotation = SpringArm->GetRelativeTransform().GetRotation().Rotator();
		NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + Val, -85.0f, 85.0f);
		SpringArm->SetRelativeRotation(NewRotation);
	}
}
