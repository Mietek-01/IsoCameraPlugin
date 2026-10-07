#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ICPlayerController.generated.h"

class UICInputControllerComponent;
class UUserWidget;

/**
 * Player controller of the showcase. Its UICInputControllerComponent sets the input mode and the mapping contexts;
 * the controller itself shows the HUD widget of the local player.
 */
UCLASS()
class ISOCAMERA_API AICPlayerController : public APlayerController
{
	GENERATED_BODY()

	// ===== Constructor, Virtual, Override =====
public:
	AICPlayerController();

protected:
	virtual void BeginPlay() override;

	// ===== UPROPERTY Variables =====
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IC|PlayerController|Components")
	TObjectPtr<UICInputControllerComponent> InputController;

	// Widget added to the viewport of the local player; no HUD when empty
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|PlayerController|Data")
	TSubclassOf<UUserWidget> HUDWidgetClass;
};
