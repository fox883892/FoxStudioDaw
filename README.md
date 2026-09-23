# FOX STUDIO DAW

FOX STUDIO DAW is a JUCE-based, cross-platform digital audio workstation foundation for recording, arranging, editing, processing, and exporting music. The project is intentionally built in small, reviewable milestones so the real-time audio path remains deterministic while the editor, project model, DSP, and future intelligent-production tools evolve independently.

## Current status

The repository currently provides a buildable DAW shell with:

- JUCE application and resizable desktop window.
- Audio device lifecycle through `AudioAppComponent`/JUCE audio utilities.
- Transport state, tempo, loop state, and timeline position.
- Track list, timeline, mixer, and piano-roll UI components.
- MIDI-note data structures and a basic synthesis/rendering path in `FoxStudioEngine`.
- XML project persistence through `ProjectManager`.
- WAV/export extension points through `ExportManager`.
- DSP processor modules for gain, EQ, compression, limiting, saturation, and chaining.
- An AI-advisor contract in `Source/AI/AiAdvisor.h` for non-real-time analysis.

This is a serious engineering foundation, not a claim that the product already matches a mature commercial DAW. The next milestones are real recording, robust project serialization, a sample-accurate edit model, production-grade DSP routing, offline rendering, plugin hosting, automated tests, and performance validation.

## Architecture

```text
JUCE application
  └─ Main.cpp / MainComponent
       ├─ TransportBar       transport controls and tempo
       ├─ TrackList          track creation and channel controls
       ├─ TimelineComponent  arrangement view and playhead
       ├─ MixerComponent     channel-strip presentation
       └─ PianoRollComponent MIDI note editing surface

FoxStudioEngine
  ├─ transport clock and tempo
  ├─ track state and MIDI data
  ├─ audio callback boundary
  └─ render/export services

ProjectManager         non-real-time XML session persistence
DSP/                   reusable audio processors and DspChain
AI/                    analysis/advisor contracts off the audio thread
ExportManager          file-format/render integration boundary
```

### Real-time rules

The audio callback must never allocate memory, lock a mutex, touch GUI components, perform file I/O, or call network/AI code. UI and AI work belongs on the message/worker threads and communicates with the engine through immutable snapshots or lock-free state where appropriate.

## Build

Requirements: CMake 3.22+, Git, and a C++20 compiler. JUCE 8.0.4 is fetched automatically by CMake.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

The generated application is placed under the platform-specific build directory. On multi-configuration generators, use `--config Release` when building and running.

## Development roadmap

1. **Build integrity** — keep every source module in the CMake target and validate clean builds on supported platforms.
2. **Engine correctness** — make transport/sample-rate handling sample-accurate and separate real-time state from editor state.
3. **Session model** — persist tracks, MIDI clips, routing, mixer state, and versioned migrations.
4. **Editing** — implement timeline selection, clips, snapping, undo/redo, and piano-roll editing against shared model data.
5. **Audio production** — add input monitoring, recording, disk-backed audio clips, channel routing, metering, and offline render.
6. **DSP and plugins** — connect per-track `DspChain`, parameter automation, and plugin hosting with safe state management.
7. **Quality** — add unit tests, render golden tests, sanitizer builds, stress tests, and latency/performance instrumentation.
8. **Intelligent tools** — run AI analysis asynchronously and expose explainable recommendations without blocking audio.

## License

See [LICENSE](LICENSE).
