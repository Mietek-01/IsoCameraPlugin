# IsoCamera

Kamera izometryczna dla Unreal Engine 5 z gotowym inputem i konfiguracją. Po uruchomieniu levelu można od razu sterować kamerą.

## Wymagania

- Unreal Engine 5.8
- Windows i Visual Studio z komponentami z pliku `.vsconfig` (Visual Studio Installer doinstaluje brakujące)

## Uruchomienie

1. Otwórz `IsoCamera.uproject`. Przy pierwszym otwarciu edytor zaproponuje zbudowanie modułu `IsoCamera` — potwierdź.
   Możesz też wygenerować projekt VS (*Generate Visual Studio project files* w menu kontekstowym `.uproject`) i zbudować konfigurację *Development Editor* / *Win64*.
2. Edytor otwiera level `LVL_Showcase`. Uruchom grę przyciskiem *Play*.

## Sterowanie

| Wejście | Działanie |
|---------|-----------|
| W A S D | przesuwanie kamery |
| kursor przy krawędzi ekranu | przesuwanie kamery |
| prawy przycisk myszy + ruch myszy | przeciąganie mapy |
| kółko myszy | przybliżanie i oddalanie |
| Q / E | obrót wokół punktu, na który patrzy kamera |
| R / F | pochylenie kamery |

Podpowiedź sterowania widać też na ekranie. Kursor jest zamknięty w oknie gry — w edytorze uwalnia go Shift+F1.

## Strojenie kamery

Prędkości, limity zoomu i pochylenia oraz wygładzanie ustawia się w DataAssecie `/Game/Systems/IsometricCameraSystem/Data/DA_IsometricCamera_Config`. Zmiany działają od razu, także w trakcie gry w edytorze. Wyjątkiem są wartości startowe (`Default…`), które kamera czyta tylko przy starcie.

## Struktura

Kod jest w module gry `IsoCamera`, w `Source/IsoCamera/`. Nagłówki leżą w `Public/`, implementacje w `Private/` pod tą samą ścieżką. Cała logika jest w C++. Blueprinty tylko konfigurują: wskazują DataAsset, akcje inputu i klasy. Klasy projektu mają prefiks `IC`.

| System | Katalog w `Public/` | Za co odpowiada |
|--------|---------------------|-----------------|
| [IsometricCameraSystem](Source/IsoCamera/Public/Systems/IsometricCameraSystem/SYSTEM_DOCS.md) | `Systems/IsometricCameraSystem/` | sterowanie kamerą: pawn kamery, komponent sterowania, DataAsset ze strojeniem |
| [InputSystem](Source/IsoCamera/Public/Systems/InputSystem/SYSTEM_DOCS.md) | `Systems/InputSystem/` | mapping contexty Enhanced Input, kursor i tryb inputu kontrolera gracza |
| [Core/Player](Source/IsoCamera/Public/Core/Player/SYSTEM_DOCS.md) | `Core/Player/` | kontroler gracza: składa input z InputSystem i pokazuje HUD |

Kamera i input nie zależą od siebie w kodzie. Łączy je konfiguracja `BP_PlayerController`, którego komponent inputu dodaje na starcie mapping context kamery.

Assety w `Content/`:
- `Systems/IsometricCameraSystem/` — pawn `BP_IsometricCamera`, DataAsset strojenia, akcje i mapping context kamery
- `Core/` — GameMode, kontroler gracza i HUD
- `Levels/LVL_Showcase` — level prezentacyjny

Jak systemy działają, co się w nich konfiguruje i jak je rozszerzać (np. kamera na własnym pawnie), opisują pliki `SYSTEM_DOCS.md` podlinkowane w tabeli.

## Licencja

Wszelkie prawa zastrzeżone.
