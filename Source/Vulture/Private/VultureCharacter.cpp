// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Vulture.h"
#include "VultureCharacter.h"
#include "VultureProjectile.h"
#include "Animation/AnimInstance.h"


//////////////////////////////////////////////////////////////////////////
// AVultureCharacter
static const int TOPFUELLEVEL = 900;

AVultureCharacter::AVultureCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// Set size for collision capsule
	CapsuleComponent->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = CapsuleComponent;
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	//Mesh1P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	//Mesh1P->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	//Mesh1P->AttachParent = FirstPersonCameraComponent;
	//Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	//Mesh1P->bCastDynamicShadow = false;
	//Mesh1P->CastShadow = false;

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AVultureCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	
	InputComponent->BindAction("Fire", IE_Pressed, this, &AVultureCharacter::OnFire);
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AVultureCharacter::TouchStarted);

	InputComponent->BindAxis("MoveForward", this, &AVultureCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AVultureCharacter::MoveRight);
	
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AVultureCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AVultureCharacter::LookUpAtRate);

	//JetPack Bind Axis
	InputComponent->BindAxis("JetPack", this, &AVultureCharacter::JetPack);

}

void AVultureCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		const FRotator SpawnRotation = GetControlRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(GunOffset);

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile at the muzzle
			World->SpawnActor<AVultureProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	//if(FireAnimation != NULL)
	//{
	//	// Get the animation object for the arms mesh
	//	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
	//	if(AnimInstance != NULL)
	//	{
	//		AnimInstance->Montage_Play(FireAnimation, 1.f);
	//	}
	//}

}

void AVultureCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// only fire for first finger down
	if (FingerIndex == 0)
	{
		OnFire();
	}
}

void AVultureCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AVultureCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AVultureCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AVultureCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


UFUNCTION()
void AVultureCharacter::JetPack(float rate){

	if (CharacterMovement && rate > 0.01)
	{
		if (fuelLevel > 0)
		{
			fuelLevel -= 10;
			FVector FinalVel = *(new FVector(GetVelocity().X, GetVelocity().Y, rate * 400));
			const FVector Velocity = GetVelocity();

			CharacterMovement->Launch(FinalVel);

			OnLaunched(FinalVel, true, true);
		}
	}
	else {
		int worldSeconds = (int)(GetWorld()->GetDeltaSeconds());
		if ((worldSeconds % 2) == 0 && fuelLevel != TOPFUELLEVEL && GetVelocity().Z == 0){
			fuelLevel += 5;
		}
	}

}
