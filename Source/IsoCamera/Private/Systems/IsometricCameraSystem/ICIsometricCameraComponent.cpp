#include "Systems/IsometricCameraSystem/ICIsometricCameraComponent.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Systems/IsometricCameraSystem/Data/ICIsometricCameraDataAsset.h"
#include "Utils/ICLogCategories.h"

UICIsometricCameraComponent::UICIsometricCameraComponent()
{
	// Enabled in BeginPlay once the config and the spring arm are found
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UICIsometricCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	USpringArmComponent* OwnerSpringArm = GetOwner()->FindComponentByClass<USpringArmComponent>();
	if (!HasConfig() || !OwnerSpringArm)
	{
		UE_LOG(LogICIsometricCameraSystem, Error, TEXT("[UICIsometricCameraComponent::BeginPlay] %s needs a Config and a spring arm on its owner (Config: %d, SpringArm: %d) - the camera will not move."),
			*GetPathName(), HasConfig(), OwnerSpringArm != nullptr);
		return;
	}
	SpringArm = OwnerSpringArm;

	// The arm does not inherit the owner's rotation, so the spawn yaw (e.g. of the PlayerStart) is passed to it here
	TargetZoomDistance = FMath::Clamp(Config->DefaultZoomDistance, Config->MinZoomDistance, Config->MaxZoomDistance);
	TargetYaw = GetOwner()->GetActorRotation().Yaw;
	TargetPitch = FMath::Clamp(Config->DefaultPitchAngle, Config->MinPitchAngle, Config->MaxPitchAngle);

	SpringArm->TargetArmLength = TargetZoomDistance;
	SpringArm->SetRelativeRotation(FRotator(TargetPitch, TargetYaw, 0.0));

	// Edge scrolling polls the cursor and the smoothing runs every frame
	SetComponentTickEnabled(true);
}

void UICIsometricCameraComponent::TickComponent(float pDeltaTime, ELevelTick pTickType, FActorComponentTickFunction* pThisTickFunction)
{
	Super::TickComponent(pDeltaTime, pTickType, pThisTickFunction);

	UpdateEdgeScroll(pDeltaTime);
	UpdateKeyPan(pDeltaTime);
	UpdateZoom(pDeltaTime);
	UpdateRotation(pDeltaTime);
}

void UICIsometricCameraComponent::BindInput(UInputComponent* pInputComponent)
{
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(pInputComponent);
	if (!EnhancedInput)
	{
		UE_LOG(LogICIsometricCameraSystem, Error, TEXT("[UICIsometricCameraComponent::BindInput] Input component of %s is not UEnhancedInputComponent - camera input will not work."), *GetPathName());
		return;
	}

	if (MoveIA)
	{
		EnhancedInput->BindAction(MoveIA, ETriggerEvent::Triggered, this, &UICIsometricCameraComponent::OnMoveInput);
	}

	if (ZoomIA)
	{
		EnhancedInput->BindAction(ZoomIA, ETriggerEvent::Triggered, this, &UICIsometricCameraComponent::OnZoomInput);
	}

	if (RotateIA)
	{
		EnhancedInput->BindAction(RotateIA, ETriggerEvent::Triggered, this, &UICIsometricCameraComponent::OnRotateInput);
	}

	if (TiltIA)
	{
		EnhancedInput->BindAction(TiltIA, ETriggerEvent::Triggered, this, &UICIsometricCameraComponent::OnTiltInput);
	}

	if (DragPanIA)
	{
		EnhancedInput->BindAction(DragPanIA, ETriggerEvent::Triggered, this, &UICIsometricCameraComponent::OnDragPanInput);
	}

	if (DragPanToggleIA)
	{
		EnhancedInput->BindAction(DragPanToggleIA, ETriggerEvent::Started, this, &UICIsometricCameraComponent::OnDragPanStarted);
		EnhancedInput->BindAction(DragPanToggleIA, ETriggerEvent::Completed, this, &UICIsometricCameraComponent::OnDragPanCompleted);
	}
}

void UICIsometricCameraComponent::OnMoveInput(const FInputActionValue& pValue)
{
	// Two keys at once (e.g. W + D) give a diagonal longer than 1, which would pan faster than MoveSpeed
	PendingMoveInput = pValue.Get<FVector2D>().GetClampedToMaxSize(1.0);
}

void UICIsometricCameraComponent::OnZoomInput(const FInputActionValue& pValue)
{
	if (!IsReady())
	{
		return;
	}

	// Positive input (mouse wheel up) shortens the arm
	TargetZoomDistance = FMath::Clamp(TargetZoomDistance - pValue.Get<float>() * Config->ZoomSpeed, Config->MinZoomDistance, Config->MaxZoomDistance);
}

void UICIsometricCameraComponent::OnRotateInput(const FInputActionValue& pValue)
{
	if (!IsReady())
	{
		return;
	}

	// A held key sends input every frame, so the per-second speed is scaled by the frame time
	TargetYaw = FRotator::NormalizeAxis(TargetYaw + pValue.Get<float>() * Config->RotationSpeed * GetWorld()->GetDeltaSeconds());
}

void UICIsometricCameraComponent::OnTiltInput(const FInputActionValue& pValue)
{
	if (!IsReady())
	{
		return;
	}

	// A held key sends input every frame, so the per-second speed is scaled by the frame time
	const float PitchStep = pValue.Get<float>() * Config->TiltSpeed * GetWorld()->GetDeltaSeconds();
	TargetPitch = FMath::Clamp(TargetPitch + PitchStep, Config->MinPitchAngle, Config->MaxPitchAngle);
}

void UICIsometricCameraComponent::OnDragPanInput(const FInputActionValue& pValue)
{
	if (!bIsDragPanning || !IsReady())
	{
		return;
	}

	if (bIsFirstDragPanFrame)
	{
		bIsFirstDragPanFrame = false;
		return;
	}

	// The map follows the mouse, so the camera moves the opposite way. DragPanSpeed is set for DefaultZoomDistance;
	// a farther camera sees more of the map per pixel, so the speed grows with the arm length
	const float ZoomScale = Config->DefaultZoomDistance > 0.f ? SpringArm->TargetArmLength / Config->DefaultZoomDistance : 1.f;
	MoveAlongCameraYaw(-pValue.Get<FVector2D>(), Config->DragPanSpeed * ZoomScale);
}

void UICIsometricCameraComponent::OnDragPanStarted()
{
	bIsDragPanning = true;
	bIsFirstDragPanFrame = true;

	// The cursor is hidden for the drag and gets its previous visibility back on release
	if (APlayerController* PlayerController = GetPlayerController())
	{
		bIsCursorShownBeforeDragPan = PlayerController->ShouldShowMouseCursor();
		PlayerController->SetShowMouseCursor(false);
	}
}

void UICIsometricCameraComponent::OnDragPanCompleted()
{
	bIsDragPanning = false;

	if (APlayerController* PlayerController = GetPlayerController())
	{
		PlayerController->SetShowMouseCursor(bIsCursorShownBeforeDragPan);
	}
}

void UICIsometricCameraComponent::UpdateEdgeScroll(float pDeltaTime)
{
	if (bIsDragPanning)
	{
		return;
	}

	const APlayerController* PlayerController = GetPlayerController();
	float MouseX = 0.f;
	float MouseY = 0.f;
	if (!PlayerController || !PlayerController->GetMousePosition(MouseX, MouseY))
	{
		return;
	}

	int32 ViewportSizeX = 0;
	int32 ViewportSizeY = 0;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
	if (ViewportSizeX == 0 || ViewportSizeY == 0)
	{
		return;
	}

	// Left and right edges pan sideways, the top edge pans forward and the bottom edge back
	const float Threshold = Config->EdgeScrollThreshold;
	FVector2D ScrollDirection = FVector2D::ZeroVector;
	if (MouseX <= Threshold)
	{
		ScrollDirection.X = -1.0;
	}
	else if (MouseX >= ViewportSizeX - Threshold)
	{
		ScrollDirection.X = 1.0;
	}

	if (MouseY <= Threshold)
	{
		ScrollDirection.Y = 1.0;
	}
	else if (MouseY >= ViewportSizeY - Threshold)
	{
		ScrollDirection.Y = -1.0;
	}

	// A screen corner scrolls along both axes; clamping keeps the diagonal at EdgeScrollSpeed instead of faster
	MoveAlongCameraYaw(ScrollDirection.GetClampedToMaxSize(1.0), Config->EdgeScrollSpeed * pDeltaTime);
}

void UICIsometricCameraComponent::UpdateKeyPan(float pDeltaTime)
{
	// Speeds up toward the held direction and slows down to a stop once the keys are released
	const bool bHasMoveInput = !PendingMoveInput.IsNearlyZero();
	const FVector2D TargetVelocity = bHasMoveInput ? PendingMoveInput : FVector2D::ZeroVector;
	const float InterpSpeed = bHasMoveInput ? Config->MoveAcceleration : Config->MoveDeceleration;
	MoveVelocity = FMath::Vector2DInterpTo(MoveVelocity, TargetVelocity, pDeltaTime, InterpSpeed);
	PendingMoveInput = FVector2D::ZeroVector;

	MoveAlongCameraYaw(MoveVelocity, Config->MoveSpeed * pDeltaTime);
}

void UICIsometricCameraComponent::UpdateZoom(float pDeltaTime)
{
	// FInterpTo lands exactly on the target once it is close enough, which ends the updates
	if (SpringArm->TargetArmLength != TargetZoomDistance)
	{
		SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetZoomDistance, pDeltaTime, Config->ZoomInterpSpeed);
	}
}

void UICIsometricCameraComponent::UpdateRotation(float pDeltaTime)
{
	const FRotator CurrentRotation = SpringArm->GetRelativeRotation();

	// Yaw turns the short way around; the pitch target is always within limits, so pitch interpolates directly
	const double YawDelta = FRotator::NormalizeAxis(TargetYaw - CurrentRotation.Yaw);
	const FRotator NewRotation(
		FMath::FInterpTo(CurrentRotation.Pitch, TargetPitch, pDeltaTime, Config->TiltInterpSpeed),
		CurrentRotation.Yaw + FMath::FInterpTo(0.0, YawDelta, pDeltaTime, Config->RotationInterpSpeed),
		0.0);

	if (!NewRotation.Equals(CurrentRotation))
	{
		SpringArm->SetRelativeRotation(NewRotation);
	}
}

void UICIsometricCameraComponent::MoveAlongCameraYaw(const FVector2D& pDirection, float pDistance)
{
	if (pDirection.IsNearlyZero())
	{
		return;
	}

	// Only the yaw counts, so the pawn moves over a flat plane whatever the pitch
	const FRotationMatrix YawMatrix(FRotator(0.0, SpringArm->GetTargetRotation().Yaw, 0.0));
	const FVector Offset = (YawMatrix.GetUnitAxis(EAxis::X) * pDirection.Y + YawMatrix.GetUnitAxis(EAxis::Y) * pDirection.X) * pDistance;
	GetOwner()->AddActorWorldOffset(Offset);
}

bool UICIsometricCameraComponent::IsReady() const
{
	return HasConfig() && SpringArm;
}

APlayerController* UICIsometricCameraComponent::GetPlayerController() const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	return OwnerPawn ? OwnerPawn->GetController<APlayerController>() : nullptr;
}
