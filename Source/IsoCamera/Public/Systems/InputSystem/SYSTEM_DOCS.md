# Systems / InputSystem

- [Cel systemu](#cel-systemu)
- [Zależności systemu](#zależności-systemu)
- [Działanie systemu](#działanie-systemu)
  - [Przepływy](#przepływy)
  - [Stan](#stan)
  - [Niezmienniki](#niezmienniki)
  - [Konfiguracja](#konfiguracja)
  - [Pułapki](#pułapki)
- [Pliki](#pliki)

## Cel systemu

Mapping contexty Enhanced Input i tryb inputu kontrolera gracza. System lokalny — `UICInputControllerComponent` siedzi na PlayerControllerze.

Dwa tryby:
- **Tryb gry** — kursor ukryty, cały input idzie do gry (`FInputModeGameOnly`).
- **Tryb UI** — kursor widoczny i zamknięty w viewporcie, input trafia do gry i do UI (`FInputModeGameAndUI`).

System nie binduje akcji. Robią to pawny i inne systemy.

## Zależności systemu

- Nie zna innych systemów projektu.
- Silnik:
  - Enhanced Input — `UEnhancedInputLocalPlayerSubsystem` lokalnego gracza.
  - Slate — `FSlateApplication`, fokus viewportu gry przy starcie w trybie UI.
- Kolejność inicjalizacji: w `BeginPlay` kontroler musi mieć już LocalPlayera. Kontroler gracza lokalnego ma go, zanim dostanie `BeginPlay`.

## Działanie systemu

### Przepływy

- **Start (`BeginPlay`):**
  - Przy `bIsGameModeOnStart` komponent dokłada `DefaultGameModeInputMaps` (bez czyszczenia), ukrywa kursor i ustawia tryb gry.
  - W przeciwnym razie włącza tryb UI. W następnym ticku daje fokus viewportowi gry, żeby klawiatura działała bez kliknięcia.
- **`EnableUIMode`:**
  1. Czyści wszystkie mapping contexty gracza i dodaje `DefaultUIModeInputMaps`.
  2. Pokazuje kursor.
  3. Ustawia tryb UI: kursor zostaje widoczny przy wciśniętym przycisku myszy i nie wychodzi z viewportu (`LockAlways`).
- **`DisableUIMode`:** przywraca contexty sprzed ostatniej zmiany (`PreviousInputMaps`), ukrywa kursor i ustawia tryb gry.
- **`SetInputMaps`:** zastępuje aktywne contexty (`pWithClear`) albo do nich dokłada. `pWithFlush` puszcza trzymane klawisze.
- **Event `InputMapsChanged`:** po każdej zmianie map, z poprzednimi i nowymi contextami.

### Stan

- Komponent trzyma aktywne contexty (`CurrentInputMaps`) i contexty sprzed ostatniej zmiany (`PreviousInputMaps`).
- Nic nie trafia do sejwa.

### Niezmienniki

- Komponent jest jedynym miejscem, które zmienia mapping contexty gracza. Zmiana z czyszczeniem woła `ClearAllMappings`, więc usuwa też contexty dodane z pominięciem komponentu.
- Wszystkie contexty mają ten sam priorytet, więc kolejność na liście nie ma znaczenia.
- `CurrentInputMaps` nie zawiera duplikatów ani pustych wpisów.

### Konfiguracja

Pola komponentu, ustawiane w BP kontrolera:

| Pole | Opis |
|------|------|
| `bIsGameModeOnStart` | Start w trybie gry (true) albo w trybie UI (false). |
| `DefaultGameModeInputMaps` | Contexty dodawane na starcie w trybie gry. |
| `DefaultUIModeInputMaps` | Contexty trybu UI, dodawane przy każdym `EnableUIMode`. |

### Pułapki

- `DisableUIMode` przywraca contexty sprzed ostatniej zmiany, a nie sprzed włączenia trybu UI. Po starcie w trybie UI to pusta lista. Po dwóch `EnableUIMode` z rzędu wracają contexty trybu UI.
- `IsUIMode` sprawdza tylko contexty: true, gdy aktywne są wszystkie z `DefaultUIModeInputMaps`. Nie patrzy na kursor ani tryb inputu. Przy pustej liście zawsze zwraca false.
- W trybie UI kursor jest zamknięty w viewporcie. W PIE kursor uwalnia Shift+F1.
- Komponent na aktorze, który nie jest PlayerControllerem, nic nie robi i loguje Warning w `LogICInputSystem`.

## Pliki

| Plik | Opis |
|------|------|
| `ICInputControllerComponent.h/.cpp` | Komponent na PlayerControllerze: mapping contexty gracza, tryby gry i UI, event `InputMapsChanged`. |
