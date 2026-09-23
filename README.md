# FOX STUDIO DAW

JUCE-based cross-platform DAW foundation. The project is developed in small, buildable milestones and every milestone is committed to this repository.

## Build

Requirements: CMake 3.22+, Git, and a C++20 compiler.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

JUCE is downloaded by CMake through `FetchContent`.

## Development milestones

1. Buildable application shell and responsive UI.
2. Audio device lifecycle and real-time callback boundary.
3. Session/project model and non-real-time save/load.
4. Tracktion Engine integration.
5. Per-track DSP, metering, MIDI editing, automation, plug-ins, and export.

The audio callback must never allocate, lock, access GUI components, perform file I/O, or run network/AI work. AI analysis belongs on a worker thread and communicates through snapshots.
