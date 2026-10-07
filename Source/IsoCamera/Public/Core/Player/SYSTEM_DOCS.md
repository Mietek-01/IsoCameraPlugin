# Core / Player

- [Cel systemu](#cel-systemu)
- [Zależności systemu](#zależności-systemu)
- [Działanie systemu](#działanie-systemu)
  - [Przepływy](#przepływy)
  - [Konfiguracja](#konfiguracja)
- [Pliki](#pliki)

## Cel systemu

Kontroler gracza prezentacji kamery. Składa input gracza z komponentu InputSystem i pokazuje HUD lokalnego gracza. Kamerą nie steruje — robi to pawn kamery.

## Zależności systemu

- InputSystem — klasa `UICInputControllerComponent`, tworzona w konstruktorze kontrolera. InputSystem nie zna `Core/Player`.
- Silnik: UMG — `UUserWidget` jako HUD.
- Assety: `BP_PlayerController` referuje `IMC_IsometricCamera_Default` z IsometricCameraSystem i `WBP_HUD`.

## Działanie systemu

### Przepływy

- **Start (`BeginPlay`):**
  1. `Super::BeginPlay` uruchamia `BeginPlay` komponentu `InputController`, który ustawia tryb inputu i mapping contexty.
  2. Kontroler gracza lokalnego tworzy widget z `HUDWidgetClass` i dodaje go do viewportu. Bez klasy HUD-a nie ma.

### Konfiguracja

| Asset | Ustawienia |
|-------|------------|
| `/Game/IsoCamera/Core/Player/BP_PlayerController` | Podklasa `AICPlayerController`. `HUDWidgetClass` = `WBP_HUD`. Komponent `InputController`: start w trybie UI (`bIsGameModeOnStart` = false), `DefaultUIModeInputMaps` = `IMC_IsometricCamera_Default`, bez contextów trybu gry. |
| `/Game/IsoCamera/Core/Player/UI/WBP_HUD` | Sam layout, bez logiki: podpowiedź sterowania (`T_CameraControlsHint`, 240×360) w prawym dolnym rogu. Obrazek nie przyjmuje myszy (`HitTestInvisible`). |

Kontrolera używa `/Game/IsoCamera/Core/Game/BP_GameMode` — domyślny GameMode projektu (`GlobalDefaultGameMode` w `Config/DefaultEngine.ini`), który spawnuje pawna `BP_IsometricCamera`.

## Pliki

| Plik | Opis |
|------|------|
| `ICPlayerController.h/.cpp` | Kontroler gracza: tworzy komponent `InputController` i pokazuje HUD lokalnego gracza (`HUDWidgetClass`). |
