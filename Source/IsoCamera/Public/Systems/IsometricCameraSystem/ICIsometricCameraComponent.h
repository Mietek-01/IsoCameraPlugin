#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ICIsometricCameraComponent.generated.h"

class APlayerController;
class UICIsometricCameraDataAsset;
class UInputAction;
class UInputComponent;
class USpringArmComponent;
struct FInputActionValue;

/**
 * Isometric camera control for its pawn: pan (move keys, screen edges, mouse drag), zoom, orbit (yaw) and tilt (pitch).
 * Panning moves the pawn; zoom, orbit and tilt drive the pawn's spring arm, smoothed toward targets set by input.
 * The pawn calls BindInput; the mapping context with the actions has to be added by the player's input setup.
 */
UCLASS(ClassGroup = (IC), meta = (BlueprintSpawnableComponent))
class ISOCAMERA_API UICIsometricCameraComponent : public UActorComponent
{
	GENERATED_BODY()

	// ===== Constructor, Virtual, Override =====
public:
	UICIsometricCameraComponent();

	virtual void TickComponent(float pDeltaTime, ELevelTick pTickType, FActorComponentTickFunction* pThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

	// ===== UPROPERTY Variables =====
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCamera|Data")
	TObjectPtr<UICIsometricCameraDataAsset> Config;

	// Axis2D: X pans right, Y pans forward. Every action is optional - an empty one turns its control off
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCamera|Data")
	TObjectPtr<UInputAction> MoveIA;

	// Axis1D: positive zooms in
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCamera|Data")
	TObjectPtr<UInputAction> ZoomIA;

	// Axis1D: positive raises the yaw (turns the view right)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCamera|Data")
	TObjectPtr<UInputAction> RotateIA;

	// Axis1D: positive raises the pitch toward the flattest view
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCamera|Data")
	TObjectPtr<UInputAction> TiltIA;

	// Axis2D: mouse movement that drags the map while DragPanToggleIA is held
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCamera|Data")
	TObjectPtr<UInputAction> DragPanIA;

	// Digital: hold to drag the map with the mouse
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCamera|Data")
	TObjectPtr<UInputAction> DragPanToggleIA;

private:
	// The owner's spring arm, found in BeginPlay; stays null when the camera could not be set up
	UPROPERTY(Transient)
	TObjectPtr<USpringArmComponent> SpringArm;

	// ===== Internal Variables =====
private:
	// Targets the spring arm is smoothed toward
	float TargetZoomDistance = 0.f;
	double TargetYaw = 0.0;
	double TargetPitch = 0.0;

	bool bIsDragPanning = false;
	// The first mouse delta of a drag can carry a jump of the cursor, so it is skipped
	bool bIsFirstDragPanFrame = false;
	// Cursor visibility restored when the drag ends
	bool bIsCursorShownBeforeDragPan = false;

	// Move input received since the last tick and the smoothed pan velocity following it (X = right, Y = forward)
	FVector2D PendingMoveInput = FVector2D::ZeroVector;
	FVector2D MoveVelocity = FVector2D::ZeroVector;

	// ===== Internal Methods =====
public:
	// Binds the camera input actions; the owning pawn calls it from SetupPlayerInputComponent
	void BindInput(UInputComponent* pInputComponent);
	bool HasConfig() const { return Config != nullptr; }

private:
	void OnMoveInput(const FInputActionValue& pValue);
	void OnZoomInput(const FInputActionValue& pValue);
	void OnRotateInput(const FInputActionValue& pValue);
	void OnTiltInput(const FInputActionValue& pValue);
	void OnDragPanInput(const FInputActionValue& pValue);
	void OnDragPanStarted();
	void OnDragPanCompleted();

	void UpdateEdgeScroll(float pDeltaTime);
	void UpdateKeyPan(float pDeltaTime);
	void UpdateZoom(float pDeltaTime);
	void UpdateRotation(float pDeltaTime);
	// Moves the pawn along the camera yaw: pDirection X = right, Y = forward, scaled by pDistance
	void MoveAlongCameraYaw(const FVector2D& pDirection, float pDistance);
	// True once BeginPlay found the config and the spring arm
	bool IsReady() const;
	// Controller of the owning pawn when it is a player controller
	APlayerController* GetPlayerController() const;
};
