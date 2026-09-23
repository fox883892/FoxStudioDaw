# Architecture and development order

## Boundaries

### UI thread
`MainComponent`, `TransportBar`, `TrackList`, `TimelineComponent`, `MixerComponent`, and `PianoRollComponent` own presentation and user interaction. They must not perform blocking audio work.

### Engine
`FoxStudioEngine` owns transport state and the real-time rendering boundary. Its public methods used by the audio callback must be allocation-free and deterministic after `prepare`.

### Non-real-time services
`ProjectManager` handles versioned session serialization. `ExportManager` handles file output and offline rendering. Both belong outside the audio callback.

### DSP
`Source/DSP` contains processors with a common `DspProcessor` contract and `DspChain` orchestration. Processors should be prepared before playback and receive parameter updates through safe, preallocated state.

### Analysis
`Source/AI/AiAdvisor.h` is an advisory layer only. Analysis must consume snapshots generated away from the audio callback and must never call UI or file/network APIs from the callback.

## Milestones

1. **Compile and smoke test**: configure, build, launch, create tracks, play, stop.
2. **Model integration**: make timeline and piano roll edit the same track/note model used by the engine.
3. **Persistence**: serialize all editable state with a schema version and migration path.
4. **Audio I/O**: add input selection, recording buffers, disk-backed clips, and monitoring.
5. **Render**: implement offline block rendering through the same engine graph and write WAV/AIFF safely.
6. **Mixing**: apply gain/pan/mute/solo and DSP chains per track, then add metering.
7. **Undo and automation**: use commands/value trees for undoable edits and parameter lanes.
8. **Validation**: unit tests for timing, serialization, DSP, clipping, and deterministic renders.

## Definition of done for a production milestone

- The code is included in `CMakeLists.txt`.
- The real-time path has no allocations, locks, GUI access, or file I/O.
- The feature has a deterministic test or smoke-test procedure.
- Project state is saved and restored where applicable.
- Failure states are surfaced to the user instead of silently simulated.
