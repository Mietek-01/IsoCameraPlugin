#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ICInputControllerComponent.generated.h"

class APlayerController;
class UInputMappingContext;
class UICInputControllerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInputMapsChangedDelegate,
	const TArray<UInputMappingContext*>&, pPreviousMaps,
	const TArray<UInputMappingContext*>&, pNewMaps,
	UICInputControllerComponent*, pInputController);

/**
 * Manages the input mapping contexts and the input mode of its player controller.
 * Game mode hides the cursor and gives all input to the game; UI mode shows the cursor and keeps it in the viewport.
 * SetInputMaps changes the contexts without changing the mode; leaving UI mode restores the contexts active when it was entered.
 */
UCLASS(ClassGroup = (IC), meta = (BlueprintSpawnableComponent))
class ISOCAMERA_API UICInputControllerComponent : public UActorComponent
{
	GENERATED_BODY()

	// ===== Constructor, Virtual, Override =====
public:
	UICInputControllerComponent();

protected:
	virtual void BeginPlay() override;

	// ===== Events =====
public:
	// The active mapping contexts changed; pNewMaps is the whole new set
	UPROPERTY(BlueprintAssignable, Category = "IC|InputController|Events")
	FInputMapsChangedDelegate InputMapsChanged;

	// ===== UPROPERTY Variables =====
protected:
	// Start in game mode (hidden cursor); otherwise start in UI mode (visible cursor)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|InputController|Data")
	bool bIsGameModeOnStart = true;

	// Mapping contexts applied when the game starts in game mode
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|InputController|Data")
	TArray<TObjectPtr<UInputMappingContext>> DefaultGameModeInputMaps;

	// Mapping contexts applied whenever UI mode is enabled
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IC|InputController|Data")
	TArray<TObjectPtr<UInputMappingContext>> DefaultUIModeInputMaps;

private:
	// Switched only by EnableUIMode and DisableUIMode; the active contexts do not affect it
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "IC|InputController|Runtime Data")
	bool bIsUIMode = false;

	UPROPERTY(VisibleInstanceOnly, Transient, Category = "IC|InputController|Runtime Data")
	TArray<TObjectPtr<UInputMappingContext>> CurrentInputMaps;

	// Mapping contexts active before the last change; InputMapsChanged reports them
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "IC|InputController|Runtime Data")
	TArray<TObjectPtr<UInputMappingContext>> PreviousInputMaps;

	// Mapping contexts active when UI mode was entered from game mode; DisableUIMode restores them
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "IC|InputController|Runtime Data")
	TArray<TObjectPtr<UInputMappingContext>> InputMapsBeforeUIMode;

	// ===== UFUNCTION Methods =====
public:
	// pWithClear replaces the active contexts instead of adding to them; pWithFlush releases keys held during the change
	UFUNCTION(BlueprintCallable, Category = "IC|InputController|Methods")
	void SetInputMaps(const TArray<UInputMappingContext*>& pNewMaps, bool pWithClear = true, bool pWithFlush = false);

	// Saves the active contexts, shows the cursor and applies the default UI mode contexts; does nothing in UI mode
	UFUNCTION(BlueprintCallable, Category = "IC|InputController|Methods")
	void EnableUIMode();

	// Hides the cursor and restores the contexts saved when UI mode was entered; does nothing in game mode
	UFUNCTION(BlueprintCallable, Category = "IC|InputController|Methods")
	void DisableUIMode();

	// True between EnableUIMode and DisableUIMode, whatever mapping contexts are active
	UFUNCTION(BlueprintPure, Category = "IC|InputController|Getters")
	bool IsUIMode() const;

	UFUNCTION(BlueprintPure, Category = "IC|InputController|Getters")
	TArray<UInputMappingContext*> GetCurrentInputMaps() const;

	// Mapping contexts active before the last change
	UFUNCTION(BlueprintPure, Category = "IC|InputController|Getters")
	TArray<UInputMappingContext*> GetPreviousInputMaps() const;

	// ===== Internal Methods =====
private:
	void InitializeInputMaps();
	// The owner as a player controller; warns when the component sits on anything else
	APlayerController* GetOwnerPlayerController() const;
};
