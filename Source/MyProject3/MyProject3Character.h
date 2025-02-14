// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyProject3Character.generated.h"

UENUM(BlueprintType)
enum class EAnimationType : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Run UMETA(DisplayName = "Run")
};

const TArray<FString> FlipbookPaths = {
	"/Game/Animations/IdleAnimation",
	"/Game/Animations/RunAnimation"
};

UCLASS(Blueprintable)
class AMyProject3Character : public ACharacter
{
	GENERATED_BODY()

public:
	AMyProject3Character();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	//FORCEINLINE class UPaperSpriteComponent* GetPlayerSprite() const { return PlayerSprite; }

	UFUNCTION(BlueprintCallable)
	void ChangeFlipbook(EAnimationType animation);

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = PlayerSprite, meta = (AllowPrivateAccess = "true"))
	//class UPaperSpriteComponent* PlayerSprite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* Flipbook; // Your Flipbook component

	UPROPERTY()
	EAnimationType CurrentAnimationState = EAnimationType::Idle;

	UPROPERTY()
	FVector LastDirection;
};

