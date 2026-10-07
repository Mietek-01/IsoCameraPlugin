#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ICIsometricCameraDataAsset.generated.h"

/**
 * Tuning of the isometric camera: pan speeds, zoom and pitch limits, rotation speeds and smoothing.
 * The camera reads it on every input and tick, so edits made while the game runs apply at once.
 */
UCLASS(BlueprintType)
class ISOCAMERA_API UICIsometricCameraDataAsset : public UDataAsset
{
	GENERATED_BODY()

	// ===== UPROPERTY Variables =====
public:
	// Pan speed of the move keys at full input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Movement", meta = (ClampMin = "0", Units = "cm/s"))
	float MoveSpeed = 1500.f;

	// How fast key panning speeds up to the held direction (interpolation speed)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Movement", meta = (ClampMin = "0"))
	float MoveAcceleration = 8.f;

	// How fast key panning slows down to a stop after the keys are released (interpolation speed)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Movement", meta = (ClampMin = "0"))
	float MoveDeceleration = 6.f;

	// Pan speed while the cursor is at a screen edge
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Movement", meta = (ClampMin = "0", Units = "cm/s"))
	float EdgeScrollSpeed = 1000.f;

	// Distance from a screen edge, in pixels, within which the cursor scrolls the camera
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Movement", meta = (ClampMin = "0"))
	float EdgeScrollThreshold = 20.f;

	// Pan distance per unit of mouse input while dragging the map, at DefaultZoomDistance.
	// It grows with the camera distance, so the map keeps pace with the cursor at any zoom
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Movement", meta = (ClampMin = "0"))
	float DragPanSpeed = 100.f;

	// Arm length change per zoom input (one mouse wheel notch)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Zoom", meta = (ClampMin = "0", Units = "cm"))
	float ZoomSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Zoom", meta = (ClampMin = "0", Units = "cm"))
	float MinZoomDistance = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Zoom", meta = (ClampMin = "0", Units = "cm"))
	float MaxZoomDistance = 8000.f;

	// Arm length at the start, kept within the zoom limits
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Zoom", meta = (ClampMin = "0", Units = "cm"))
	float DefaultZoomDistance = 2000.f;

	// How fast the arm length follows the zoom input (interpolation speed)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Zoom", meta = (ClampMin = "0"))
	float ZoomInterpSpeed = 6.f;

	// Yaw speed while the rotate input is held
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Rotation", meta = (ClampMin = "0", Units = "deg/s"))
	float RotationSpeed = 120.f;

	// How fast the camera yaw follows the rotate input (interpolation speed)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Rotation", meta = (ClampMin = "0"))
	float RotationInterpSpeed = 8.f;

	// Pitch speed while the tilt input is held
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Tilt", meta = (ClampMin = "0", Units = "deg/s"))
	float TiltSpeed = 120.f;

	// Steepest view; negative pitch looks down, -90 straight down
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Tilt", meta = (ClampMin = "-90", ClampMax = "90", Units = "deg"))
	float MinPitchAngle = -70.f;

	// Flattest view
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Tilt", meta = (ClampMin = "-90", ClampMax = "90", Units = "deg"))
	float MaxPitchAngle = -20.f;

	// Pitch at the start, kept within the pitch limits
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Tilt", meta = (ClampMin = "-90", ClampMax = "90", Units = "deg"))
	float DefaultPitchAngle = -45.f;

	// How fast the camera pitch follows the tilt input (interpolation speed)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|IsometricCameraConfig|Tilt", meta = (ClampMin = "0"))
	float TiltInterpSpeed = 6.f;
};
