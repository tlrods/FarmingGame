// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProject3PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "MyProject3Character.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AMyProject3PlayerController::AMyProject3PlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AMyProject3PlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AMyProject3PlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AMyProject3PlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AMyProject3PlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AMyProject3PlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AMyProject3PlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AMyProject3PlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AMyProject3PlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AMyProject3PlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AMyProject3PlayerController::OnTouchReleased);

		InputComponent->BindAction("KeyPressedAction", EInputEvent::IE_Pressed, this, &AMyProject3PlayerController::KeyPressed);

		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AMyProject3PlayerController::OnMoveRight);
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AMyProject3PlayerController::OnMoveForward);

		//InputComponent->BindAxis("MoveRight", this, &AMyProject3PlayerController::MoveRight);
		//InputComponent->BindAxis("MoveForward", this, &AMyProject3PlayerController::MoveForward);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMyProject3PlayerController::OnInputStarted()
{
	//StopMovement();
}

// Triggered every frame when the input is held down
void AMyProject3PlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AMyProject3PlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void AMyProject3PlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AMyProject3PlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void AMyProject3PlayerController::MoveRight(float XAxis)
{
	APawn* ControlledPawn = GetPawn();
	
	FRotator Rotation = ControlledPawn->GetController()->GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	ControlledPawn->AddMovementInput(RightDirection, XAxis);
}

void AMyProject3PlayerController::MoveForward(float ZAxis)
{
	APawn* ControlledPawn = GetPawn();

	FRotator Rotation = ControlledPawn->GetController()->GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	ControlledPawn->AddMovementInput(ForwardDirection, ZAxis);
}

void AMyProject3PlayerController::OnMoveRight()
{
	APawn* ControlledPawn = GetPawn();

	FRotator Rotation = ControlledPawn->GetController()->GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	ControlledPawn->AddMovementInput(RightDirection);
}

void AMyProject3PlayerController::OnMoveForward()
{
	APawn* ControlledPawn = GetPawn();

	FRotator Rotation = ControlledPawn->GetController()->GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	ControlledPawn->AddMovementInput(ForwardDirection);
}

void AMyProject3PlayerController::KeyPressed(FKey fKey)
{
	APawn* ControlledPawn = GetPawn();

	if(WasInputKeyJustPressed(EKeys::AnyKey))
	{
		bool b = true;
	}
}

void AMyProject3PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* ControlledPawn = GetPawn();

	if (IsInputKeyDown(EKeys::W))
	{
		ControlledPawn->AddMovementInput(FVector::ForwardVector, 1.0f, false);
	}
	if (IsInputKeyDown(EKeys::A))
	{
		ControlledPawn->AddMovementInput(FVector::LeftVector, 1.0f, false);
	}
	if (IsInputKeyDown(EKeys::S))
	{
		ControlledPawn->AddMovementInput(FVector::BackwardVector, 1.0f, false);
	}
	if (IsInputKeyDown(EKeys::D))
	{
		ControlledPawn->AddMovementInput(FVector::RightVector, 1.0f, false);
	}
}