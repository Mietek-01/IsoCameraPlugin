#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ICIsometricCameraPawn.generated.h"

class UCameraComponent;
class UICIsometricCameraComponent;
class USceneComponent;
class USpringArmComponent;

/**
 * Ready-made isometric camera: a pivot on the map, a spring arm holding the camera at a distance and angle,
 * and UICIsometricCameraComponent that moves the pivot and drives the arm from player input.
 */
UCLASS()
class ISOCAMERA_API AICIsometricCameraPawn : public APawn
{
	GENERATED_BODY()

	// ===== Constructor, Virtual, Override =====
public:
	AICIsometricCameraPawn();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* pPlayerInputComponent) override;

	// ===== UPROPERTY Variables =====
protected:
	// Point on the map the camera looks at; panning moves it
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IC|IsometricCameraPawn|Components")
	TObjectPtr<USceneComponent> RootPivot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IC|IsometricCameraPawn|Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IC|IsometricCameraPawn|Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IC|IsometricCameraPawn|Components")
	TObjectPtr<UICIsometricCameraComponent> IsometricCamera;
};
