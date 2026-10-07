#include "Core/Player/ICPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Systems/InputSystem/ICInputControllerComponent.h"

AICPlayerController::AICPlayerController()
{
	InputController = CreateDefaultSubobject<UICInputControllerComponent>(TEXT("InputController"));
}

void AICPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalPlayerController() || !HUDWidgetClass)
	{
		return;
	}

	if (UUserWidget* HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass))
	{
		HUDWidget->AddToViewport();
	}
}
