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
 * Leaving UI mode restores the mapping contexts that were active before the last change.
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
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "IC|InputController|Runtime Data")
	TArray<TObjectPtr<UInputMappingContext>> CurrentInputMaps;

	// Mapping contexts active before the last change; DisableUIMode restores them
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "IC|InputController|Runtime Data")
	TArray<TObjectPtr<UInputMappingContext>> PreviousInputMaps;

	// ===== UFUNCTION Methods =====
public:
	// pWithClear replaces the active contexts instead of adding to them; pWithFlush releases keys held during the change
	UFUNCTION(BlueprintCallable, Category = "IC|InputController|Methods")
	void SetInputMaps(const TArray<UInputMappingContext*>& pNewMaps, bool pWithClear = true, bool pWithFlush = false);

	UFUNCTION(BlueprintCallable, Category = "IC|InputController|Methods")
	void EnableUIMode();

	UFUNCTION(BlueprintCallable, Category = "IC|InputController|Methods")
	void DisableUIMode();

	// True when every default UI mode context is active
	UFUNCTION(BlueprintPure, Category = "IC|InputController|Getters")
	bool IsUIMode() const;

	// ===== Internal Methods =====
public:
	const TArray<TObjectPtr<UInputMappingContext>>& GetCurrentInputMaps() const { return CurrentInputMaps; }
	const TArray<TObjectPtr<UInputMappingContext>>& GetPreviousInputMaps() const { return PreviousInputMaps; }

private:
	void InitializeInputMaps();
	// pNewMaps is taken by value: callers pass this component's own arrays, which this method rewrites
	void ApplyInputMaps(TArray<TObjectPtr<UInputMappingContext>> pNewMaps, bool pWithClear, bool pWithFlush);
	// The owner as a player controller; warns when the component sits on anything else
	APlayerController* GetOwnerPlayerController() const;
};
