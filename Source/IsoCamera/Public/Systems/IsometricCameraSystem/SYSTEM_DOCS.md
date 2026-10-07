# Systems / IsometricCameraSystem

- [Cel systemu](#cel-systemu)
- [Zależności systemu](#zależności-systemu)
- [Działanie systemu](#działanie-systemu)
  - [Przepływy](#przepływy)
  - [Stan](#stan)
  - [Niezmienniki](#niezmienniki)
  - [Konfiguracja](#konfiguracja)
  - [Rozszerzanie](#rozszerzanie)
  - [Pułapki](#pułapki)
- [Pliki](#pliki)

## Cel systemu

Kamera izometryczna sterowana przez gracza. System lokalny — logika siedzi w komponencie na pawnie.

Części systemu:
- `UICIsometricCameraComponent` — sterowanie: przesuwanie (klawisze, krawędzie ekranu, przeciąganie myszą), zoom, obrót wokół punktu (yaw) i pochylenie (pitch).
- `AICIsometricCameraPawn` — gotowy pawn: pivot, spring arm, kamera i komponent sterowania.
- `UICIsometricCameraDataAsset` — strojenie: prędkości, limity, wygładzanie.

System nie dodaje mapping contextu i nie ustawia trybu inputu ani kursora. Wyjątek: na czas przeciągania mapy ukrywa kursor.

## Zależności systemu

- Nie zna innych systemów projektu.
- Mapping context z akcjami kamery (`IMC_IsometricCamera_Default`) musi dodać to, co zarządza inputem gracza. W projekcie robi to InputSystem na `BP_PlayerController`.
- Silnik:
  - Enhanced Input — bindowanie akcji do input componentu pawna.
  - `USpringArmComponent` na właścicielu komponentu — zoom i obrót.
  - `APlayerController` pawna — pozycja kursora, rozmiar viewportu, widoczność kursora.
- Kolejność inicjalizacji: komponent szuka spring armu w `BeginPlay`, więc spring arm musi istnieć wcześniej (konstruktor albo BP).

## Działanie systemu

### Przepływy

- **Bindowanie:** pawn w `SetupPlayerInputComponent` woła `BindInput`. Komponent binduje tylko akcje, które są ustawione.
- **Start (`BeginPlay`):**
  1. Komponent znajduje spring arm właściciela.
  2. Ustawia długość ramienia z `DefaultZoomDistance` i pitch z `DefaultPitchAngle`, obie przycięte do limitów.
  3. Yaw bierze z obrotu pawna przy spawnie, czyli z PlayerStartu.
  4. Włącza Tick.
- **Klawisze ruchu (`MoveIA`):** input zapisuje kierunek. Tick rozpędza prędkość do tego kierunku (`MoveAcceleration`), a po puszczeniu klawiszy wyhamowuje ją (`MoveDeceleration`). Pawn przesuwa się z `MoveSpeed`.
- **Krawędzie ekranu:** Tick czyta pozycję kursora. Kursor w pasie `EdgeScrollThreshold` pikseli od krawędzi przesuwa pawna z `EdgeScrollSpeed`. W trakcie przeciągania krawędzie nie działają.
- **Przeciąganie mapy:**
  1. Wciśnięcie `DragPanToggleIA` włącza przeciąganie i ukrywa kursor.
  2. Każdy ruch myszy (`DragPanIA`) przesuwa pawna w przeciwną stronę. Pierwszy ruch po wciśnięciu jest pomijany, bo bywa skokiem kursora.
  3. `DragPanSpeed` obowiązuje przy `DefaultZoomDistance`. Przy innym zoomie mnoży się przez stosunek długości ramienia do `DefaultZoomDistance`, więc mapa nadąża za kursorem przy każdym zoomie.
  4. Puszczenie przywraca widoczność kursora sprzed wciśnięcia.
- **Zoom, obrót, pochylenie:** input zmienia cel (długość ramienia, yaw, pitch).
  - Zoom zmienia cel o `ZoomSpeed` na jeden input (ząbek kółka).
  - Trzymany klawisz obrotu i pochylenia zmienia cel z prędkością `RotationSpeed` i `TiltSpeed` w stopniach na sekundę. Krok jest mnożony przez czas klatki, więc prędkość nie zależy od FPS.
  - Cel zoomu i pitchu jest przycinany do limitów z DA.
  - Tick wygładza ramię do celu (`ZoomInterpSpeed`, `RotationInterpSpeed`, `TiltInterpSpeed`). Yaw obraca się krótszą drogą.
- **Kierunek ruchu:** liczony z yaw kamery, więc „do przodu” to zawsze kierunek patrzenia. Pawn przesuwa się po płaszczyźnie na wysokości spawnu.

### Stan

- Komponent trzyma cele zoomu, yaw i pitchu, prędkość ruchu z klawiszy i stan przeciągania. Pozycja kamery to pozycja pawna.
- Nic nie trafia do sejwa. Każda sesja zaczyna od wartości startowych z DA.

### Niezmienniki

- Spring arm nie dziedziczy obrotu właściciela: `bInheritPitch`, `bInheritYaw`, `bInheritRoll` = false, bez `bUsePawnControlRotation`. Komponent ustawia obrót względny ramienia jako jego obrót w świecie. `AICIsometricCameraPawn` ustawia to w konstruktorze. Na innym pawnie obrót pawna składałby się z obrotem kamery.
- Cele zoomu i pitchu są zawsze w limitach z DA.
- Kierunek przesuwania z klawiszy i z krawędzi ekranu ma długość najwyżej 1. Bez tego skos (W + D, róg ekranu) byłby √2 razy szybszy niż ruch prosto.
- Bez `Config` albo bez spring armu komponent loguje Error w `BeginPlay` (`LogICIsometricCameraSystem`), nie włącza Ticka i ignoruje input.

### Konfiguracja

- `/Game/Systems/IsometricCameraSystem/Data/DA_IsometricCamera_Config` (`UICIsometricCameraDataAsset`) — wszystkie prędkości, limity i wygładzanie, w kategoriach `Movement`, `Zoom`, `Rotation`, `Tilt`. Komponent czyta DA przy każdym inpucie i ticku, więc zmiana w trakcie gry działa od razu. Wyjątek: `DefaultZoomDistance` i `DefaultPitchAngle` ustawiają kamerę tylko w `BeginPlay`.
- `/Game/Systems/IsometricCameraSystem/BP_IsometricCamera` — podklasa `AICIsometricCameraPawn`. Na komponencie `IsometricCamera` ma przypięte `Config` i sześć akcji.
- Akcje i mapping context leżą w `/Game/Systems/IsometricCameraSystem/Input/`. Mapowania w `IMC_IsometricCamera_Default`:

| Klawisz | Akcja | Typ wartości | Modyfikatory |
|---------|-------|--------------|--------------|
| W | `IA_IsometricCamera_Move` | Axis2D | Swizzle YXZ |
| S | `IA_IsometricCamera_Move` | Axis2D | Swizzle YXZ, Negate |
| D | `IA_IsometricCamera_Move` | Axis2D | — |
| A | `IA_IsometricCamera_Move` | Axis2D | Negate |
| Mouse Wheel Axis | `IA_IsometricCamera_Zoom` | Axis1D | — |
| Q | `IA_IsometricCamera_Rotate` | Axis1D | — |
| E | `IA_IsometricCamera_Rotate` | Axis1D | Negate |
| F | `IA_IsometricCamera_Tilt` | Axis1D | — |
| R | `IA_IsometricCamera_Tilt` | Axis1D | Negate |
| Mouse XY 2D-Axis | `IA_IsometricCamera_DragPan` | Axis2D | — |
| Right Mouse Button | `IA_IsometricCamera_DragPanToggle` | Digital | — |

### Rozszerzanie

- **Nowe sterowanie kamery:** IA w `Input/`, mapowanie w `IMC_IsometricCamera_Default`, pole `...IA` na komponencie z bindem w `BindInput`, wartości strojenia w `UICIsometricCameraDataAsset`.
- **Kamera na innym pawnie:** dodaj `UICIsometricCameraComponent` i spring arm bez dziedziczenia obrotu, a w `SetupPlayerInputComponent` pawna wywołaj `BindInput`.

### Pułapki

- Puste pole akcji na komponencie po cichu wyłącza dane sterowanie — bez wpisu w logu.
- Bez mapping contextu dodanego przez input gracza akcje nie przychodzą, też bez wpisu w logu.
- Krawędzie ekranu potrzebują widocznego kursora nad viewportem. Przy ukrytym, przechwyconym kursorze (tryb GameOnly) pozycja kursora nie dochodzi do krawędzi.
- Pawn jedzie na stałej wysokości, a ramię nie ma kolizji. Na pagórkowatym terenie kamera przy małym zoomie może wejść w teren.

## Pliki

| Plik | Opis |
|------|------|
| `ICIsometricCameraComponent.h/.cpp` | Sterowanie kamerą: bindowanie akcji, przesuwanie pawna, zoom, obrót i pochylenie ramienia z wygładzaniem w Ticku. |
| `ICIsometricCameraPawn.h/.cpp` | Gotowy pawn kamery: pivot jako root, spring arm bez dziedziczenia obrotu, kamera na końcu ramienia i komponent sterowania. Przekazuje input componentowi `BindInput`. |
| `Data/ICIsometricCameraDataAsset.h/.cpp` | Strojenie kamery (`DA_IsometricCamera_Config`). |
