// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProject3Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "PaperSpriteComponent.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"

AMyProject3Character::AMyProject3Character()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	CameraBoom->bUsePawnControlRotation = false;

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//Paper2D Sprite Info
	//PlayerSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("PlayerSprite"));
	//PlayerSprite->SetupAttachment(RootComponent);
	//PlayerSprite->SetUsingAbsoluteRotation(true); 
	//PlayerSprite->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

	Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
	Flipbook->SetupAttachment(RootComponent);
	Flipbook->SetUsingAbsoluteRotation(true);
	Flipbook->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));


	//FSpriteAssetInitParameters SpriteInit;
	//static ConstructorHelpers::FObjectFinder<UTexture2D> TextureFinder(TEXT("Content/Assets/rember.uasset"));
	//SpriteInit.Texture = TextureFinder.Object;
	//SpriteInit.Dimension = FIntPoint(260, 480);
	//PlayerSprite->InitializeSprite(SpriteInit);
}

void AMyProject3Character::Tick(float DeltaSeconds)
{
	bool isMoving = !GetCharacterMovement()->Velocity.IsZero();

	if (isMoving && CurrentAnimationState != EAnimationType::Run)
	{
		CurrentAnimationState = EAnimationType::Run;
		ChangeFlipbook(EAnimationType::Run);
	}

	if (!isMoving && CurrentAnimationState != EAnimationType::Idle)
	{
		CurrentAnimationState = EAnimationType::Idle;
		ChangeFlipbook(EAnimationType::Idle);
	}

	FVector CurrentVelocity = GetCharacterMovement()->Velocity;

	if (CurrentVelocity.Y != 0.0)
	{
		if (CurrentVelocity.Y > 0.0)
		{
			FRotator NewRotation(0.0, 90.0, 0.0);
			Flipbook->SetWorldRotation(NewRotation);
		}
		if (CurrentVelocity.Y < 0.0)
		{
			FRotator NewRotation(0.0, -90.0, 0.0);
			Flipbook->SetWorldRotation(NewRotation);
		}
	}

    Super::Tick(DeltaSeconds);
}

void AMyProject3Character::ChangeFlipbook(EAnimationType animation)
{
	FString FlipbookPath = FlipbookPaths[static_cast<int32>(animation)];

	switch (animation)
	{
		case EAnimationType::Run:
		{
			UPaperFlipbook* LoadedFlipbook = LoadObject<UPaperFlipbook>(nullptr, *FlipbookPath);
			Flipbook->SetFlipbook(LoadedFlipbook);
			break;
		}
		case EAnimationType::Idle:
		{
			UPaperFlipbook* LoadedFlipbook = LoadObject<UPaperFlipbook>(nullptr, *FlipbookPath);
			Flipbook->SetFlipbook(LoadedFlipbook);
			break;
		}
	}
}
