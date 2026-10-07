#include "Systems/InputSystem/ICInputControllerComponent.h"

#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputSubsystems.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "TimerManager.h"
#include "Utils/ICLogCategories.h"

UICInputControllerComponent::UICInputControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UICInputControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeInputMaps();
}

void UICInputControllerComponent::SetInputMaps(const TArray<UInputMappingContext*>& pNewMaps, bool pWithClear, bool pWithFlush)
{
	APlayerController* PlayerController = GetOwnerPlayerController();
	if (!PlayerController)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (!InputSubsystem)
	{
		UE_LOG(LogICInputSystem, Error, TEXT("[UICInputControllerComponent::SetInputMaps] %s has no EnhancedInputLocalPlayerSubsystem - input maps were not changed."), *GetPathName());
		return;
	}

	PreviousInputMaps = CurrentInputMaps;
	if (pWithClear)
	{
		InputSubsystem->ClearAllMappings();
		CurrentInputMaps.Reset();
	}

	// All contexts share one priority, so their order in the list does not matter
	for (UInputMappingContext* InputMap : pNewMaps)
	{
		if (InputMap)
		{
			InputSubsystem->AddMappingContext(InputMap, 0);
			CurrentInputMaps.AddUnique(InputMap);
		}
	}

	if (pWithFlush)
	{
		PlayerController->FlushPressedKeys();
	}

	InputMapsChanged.Broadcast(GetPreviousInputMaps(), GetCurrentInputMaps(), this);
}

void UICInputControllerComponent::EnableUIMode()
{
	APlayerController* PlayerController = GetOwnerPlayerController();
	if (!PlayerController || bIsUIMode)
	{
		return;
	}

	// The game mode contexts are saved for DisableUIMode
	InputMapsBeforeUIMode = CurrentInputMaps;
	bIsUIMode = true;

	SetInputMaps(TArray<UInputMappingContext*>(DefaultUIModeInputMaps));

	// The cursor stays visible while a mouse button is held and cannot leave the viewport
	PlayerController->SetShowMouseCursor(true);
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	PlayerController->SetInputMode(InputMode);
}

void UICInputControllerComponent::DisableUIMode()
{
	APlayerController* PlayerController = GetOwnerPlayerController();
	if (!PlayerController || !bIsUIMode)
	{
		return;
	}

	bIsUIMode = false;
	SetInputMaps(TArray<UInputMappingContext*>(InputMapsBeforeUIMode));
	// The saved contexts are used once; the list stays empty while in game mode
	InputMapsBeforeUIMode.Reset();

	PlayerController->SetShowMouseCursor(false);
	PlayerController->SetInputMode(FInputModeGameOnly());
}

bool UICInputControllerComponent::IsUIMode() const
{
	return bIsUIMode;
}

TArray<UInputMappingContext*> UICInputControllerComponent::GetCurrentInputMaps() const
{
	return TArray<UInputMappingContext*>(CurrentInputMaps);
}

TArray<UInputMappingContext*> UICInputControllerComponent::GetPreviousInputMaps() const
{
	return TArray<UInputMappingContext*>(PreviousInputMaps);
}

void UICInputControllerComponent::InitializeInputMaps()
{
	APlayerController* PlayerController = GetOwnerPlayerController();
	if (!PlayerController)
	{
		return;
	}

	if (bIsGameModeOnStart)
	{
		SetInputMaps(TArray<UInputMappingContext*>(DefaultGameModeInputMaps), false);
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());
		return;
	}

	EnableUIMode();

	// UI mode does not focus the game viewport, so keys would wait for the first click.
	// The focus is set on the next tick, once the game has finished starting up
	GetWorld()->GetTimerManager().SetTimerForNextTick([]()
	{
		FSlateApplication::Get().SetAllUserFocusToGameViewport();
	});
}

APlayerController* UICInputControllerComponent::GetOwnerPlayerController() const
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	if (!PlayerController)
	{
		UE_LOG(LogICInputSystem, Warning, TEXT("[UICInputControllerComponent::GetOwnerPlayerController] %s is not on a PlayerController - input maps and modes are not applied."), *GetPathName());
	}
	return PlayerController;
}
