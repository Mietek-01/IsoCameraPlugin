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

Tryb oznacza tylko widoczność kursora, a nie to, co robi gracz. Menu pauzy to nadal tryb UI, tylko z inną mapą inputu (`SetInputMaps`).

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
- **`EnableUIMode`:** w trybie UI nic nie robi. W trybie gry:
  1. Zapamiętuje aktywne contexty, żeby `DisableUIMode` mógł je przywrócić.
  2. Czyści wszystkie mapping contexty gracza i dodaje `DefaultUIModeInputMaps`.
  3. Pokazuje kursor.
  4. Ustawia tryb UI: kursor zostaje widoczny przy wciśniętym przycisku myszy i nie wychodzi z viewportu (`LockAlways`).
- **`DisableUIMode`:** w trybie gry nic nie robi. W trybie UI przywraca contexty zapamiętane przy wejściu w tryb UI, ukrywa kursor i ustawia tryb gry.
- **`SetInputMaps`:** zastępuje aktywne contexty (`pWithClear`) albo do nich dokłada. `pWithFlush` puszcza trzymane klawisze. Nie rusza trybu ani contextów zapamiętanych przez `EnableUIMode`.
- **Event `InputMapsChanged`:** po każdej zmianie map, z poprzednimi i nowymi contextami.

### Stan

- Komponent trzyma:
  - tryb (`bIsUIMode`), który zmieniają tylko `EnableUIMode` i `DisableUIMode`;
  - aktywne contexty (`CurrentInputMaps`) i contexty sprzed ostatniej zmiany (`PreviousInputMaps`);
  - contexty trybu gry zapamiętane przy wejściu w tryb UI (`InputMapsBeforeUIMode`). W trybie gry ta lista jest pusta.
- BP czyta stan przez `IsUIMode`, `GetCurrentInputMaps` i `GetPreviousInputMaps`. Gettery map zwracają kopie.
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
| `DefaultUIModeInputMaps` | Contexty ustawiane przy wejściu w tryb UI. |

### Pułapki

- Po starcie w trybie UI nie było aktywnych contextów gry, więc `DisableUIMode` zostawia pustą listę.
- Powrotu z innej mapy w trybie UI (np. z menu pauzy) nie da się zrobić przez `EnableUIMode`. Trzeba `SetInputMaps` z mapami do przywrócenia, np. pobranymi przez `GetCurrentInputMaps` przed otwarciem menu.
- Kursor albo tryb inputu zmienione z pominięciem komponentu nie przełączają trybu. `IsUIMode` dalej zwraca poprzednią wartość.
- W trybie UI kursor jest zamknięty w viewporcie. W PIE kursor uwalnia Shift+F1.
- Komponent na aktorze, który nie jest PlayerControllerem, nic nie robi i loguje Warning w `LogICInputSystem`.

## Pliki

| Plik | Opis |
|------|------|
| `ICInputControllerComponent.h/.cpp` | Komponent na PlayerControllerze: mapping contexty gracza, tryby gry i UI, event `InputMapsChanged`. |
