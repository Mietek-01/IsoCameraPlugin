#include "Systems/IsometricCameraSystem/ICIsometricCameraPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Systems/IsometricCameraSystem/ICIsometricCameraComponent.h"

AICIsometricCameraPawn::AICIsometricCameraPawn()
{
	// The pawn has nothing to do per frame; UICIsometricCameraComponent ticks on its own
	PrimaryActorTick.bCanEverTick = false;

	RootPivot = CreateDefaultSubobject<USceneComponent>(TEXT("RootPivot"));
	SetRootComponent(RootPivot);

	// UICIsometricCameraComponent sets the arm rotation as a world rotation, so nothing is inherited from the pivot.
	// Without the collision test the arm keeps its length when it passes through terrain or buildings
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootPivot);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	IsometricCamera = CreateDefaultSubobject<UICIsometricCameraComponent>(TEXT("IsometricCamera"));
}

void AICIsometricCameraPawn::SetupPlayerInputComponent(UInputComponent* pPlayerInputComponent)
{
	Super::SetupPlayerInputComponent(pPlayerInputComponent);

	IsometricCamera->BindInput(pPlayerInputComponent);
}
