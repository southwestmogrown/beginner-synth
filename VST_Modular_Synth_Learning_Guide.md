# VST Plugin Learning Guide (Modular Synth Track)

Date: February 27, 2026

## Goal

Build your own VST3 synthesizer while learning core real-time DSP and plugin architecture principles.

You want to do the coding yourself. This guide is structured to steer implementation decisions, explain why they matter, and provide concrete checkpoints.

---

## Why this path

- Start with C++ and JUCE for VST3 development.
- Begin with a small subtractive synth voice, then evolve into modular routing.
- Learn DSP in practical increments: oscillator, envelope, filter, modulation, voice handling.

This sequence keeps scope manageable while teaching production-relevant concepts.

---

## Day 1 Objective

Create a minimal playable synth voice:

- MIDI note in -> audible tone out
- One oscillator (sine is fine)
- ADSR envelope
- Output gain with click-free behavior

Success criteria:

- Note-on plays tone.
- Note-off releases smoothly.
- No obvious clicks at note transitions.
- CPU usage remains low.

---

## Environment Setup (high-level)

- C++ compiler (clang or gcc)
- CMake
- JUCE
- A VST3 host for testing (for example REAPER)

Start with a minimal JUCE Audio Plug-In project targeting VST3.

---

## Core DSP formulas (first voice)

- Phase increment:

  inc = f / fs

- Phase update each sample:

  phase = phase + inc

  if phase >= 1.0 then phase = phase - 1.0

- Sine output:

  x[n] = sin(2 _ pi _ phase)

- Per-sample voice output:

  y[n] = oscillator[n] _ envelope[n] _ gain[n]

Use time-based ramps for envelope and gain smoothing to avoid discontinuities.

---

## Minimal architecture

Create these parts and keep responsibilities separated:

1. SynthPluginProcessor

- Owns parameters and voice list
- Routes MIDI events
- Clears and fills output buffer

2. Voice

- Represents one playable note
- Owns oscillator + envelope + per-voice gain
- Renders samples when active

3. Oscillator

- Holds phase and phase increment
- Generates one waveform sample at a time

4. EnvelopeADSR

- Maintains ADSR state machine
- Produces a [0, 1] gain multiplier per sample

5. Smoothed gain utility

- Smooths abrupt gain/parameter changes

Keep DSP engine logic independent from UI logic.

---

## Class contract checklist (what each class should expose)

### Oscillator

- prepare(sampleRate)
- setFrequency(hz)
- renderSample() -> float

Internal state:

- sampleRate
- phase in [0, 1)
- phaseIncrement

### EnvelopeADSR

- prepare(sampleRate)
- setParameters(attack, decay, sustain, release)
- noteOn()
- noteOff()
- renderSample() -> float in [0, 1]
- isActive() -> bool

Internal state:

- currentState (idle/attack/decay/sustain/release)
- currentLevel
- per-state increments

### Voice

- startNote(midiNote, velocity)
- stopNote(allowTailOff)
- render(audioBlock, startSample, numSamples)
- isActive() -> bool
- getCurrentNote() -> int (or equivalent)

Internal state:

- note id
- oscillator
- envelope
- voiceGain
- active flag

### SynthPluginProcessor

- prepareToPlay(sampleRate, blockSize)
- processBlock(buffer, midi)

Responsibilities in processBlock:

- consume MIDI events
- dispatch note on/off to voices
- clear output buffer
- render active voices
- apply master gain (smoothed)

---

## Voice allocation rule (simple, deterministic)

- On note-on: choose first inactive voice.
- If none inactive: steal one (start with oldest or currently released voice).
- On note-off: release matching voice for that note.

Keep this simple at first. Advanced stealing policies can come later.

---

## Anti-click and real-time safety guardrails

### Anti-click

- Never hard-jump amplitude from 0 to 1 or 1 to 0.
- Use envelope attack/release with sample-time ramps.
- Use smoothing for gain/parameter transitions (for example 5 to 20 ms).

### Real-time audio safety

- No dynamic allocations in audio callback.
- No locks/mutex waits in audio callback.
- No file I/O in audio callback.
- Avoid expensive operations per sample when possible.

### Stability

- Recompute coefficients/increments correctly on sample-rate change.
- Guard against denormals in very small signal regions.

---

## Ordered fill-in-the-blanks implementation plan

Use this as your coding sequence.

### Step 1: Skeleton compile pass

- Create class files and method signatures only.
- Wire processor -> voice container -> render loop.
- Make sure project builds before DSP details.

Check:

- Plugin loads in host.
- No audio yet is acceptable at this step.

### Step 2: Oscillator implementation

Implement:

- sampleRate storage in prepare
- setFrequency computes phaseIncrement = frequency / sampleRate
- renderSample returns sine from current phase and wraps phase

Edge cases:

- If sampleRate <= 0, fail-safe with zero output.
- Clamp or ignore invalid frequencies (<= 0).

Check:

- Fixed test frequency produces stable tone when called repeatedly.

### Step 3: ADSR implementation

Implement states:

- idle -> attack -> decay -> sustain -> release -> idle

Implement per-sample level updates:

- attack ramps upward to 1
- decay ramps downward to sustain
- sustain holds
- release ramps to 0

Edge cases:

- Very short attack/release should not click (minimum ramp in samples).
- sustain stays in [0, 1].

Check:

- noteOn triggers attack.
- noteOff transitions to release and eventually inactive.

### Step 4: Voice implementation

Implement startNote:

- store midi note
- convert midi note to frequency
- set oscillator frequency
- set voice gain from velocity (scaled safely)
- trigger envelope noteOn
- mark active

Implement stopNote:

- if tailOff allowed -> envelope noteOff
- else force idle and inactive

Implement render:

- for each sample, generate osc _ env _ gain
- add to output channels
- if envelope inactive, mark voice inactive

Check:

- Single note plays and releases cleanly.

### Step 5: MIDI event routing in processor

In processBlock:

- clear output first
- parse MIDI events in timestamp order
- dispatch note on/off to voice manager
- render all active voices for block

Check:

- MIDI note-on starts sound.
- MIDI note-off releases sound.
- Repeated triggers do not stick notes.

### Step 6: Gain smoothing and safety trim

Implement:

- smoothed master gain (or voice gain)
- optional output limiter/clamp for learning-stage safety

Check:

- No zipper noise when changing gain parameter.

### Step 7: Polyphony sanity

Implement minimal allocator:

- fixed number of voices (for example 8)
- first inactive allocation
- fallback steal policy

Check:

- Two or more simultaneous notes are audible.
- Voice stealing behaves predictably under overload.

---

## Day 1 test checklist

Run this exact checklist after coding:

- Note-on plays correct pitch.
- Note-off release is smooth.
- Fast repeated note taps are click-free.
- Basic polyphony works.
- CPU remains stable during sustained notes.
- No stuck notes after rapid play.

---

## Common mistakes to avoid

- Allocating memory in audio processing callback.
- Forgetting to update DSP internals on sample-rate changes.
- Hard resets of amplitude or gain (causes clicks).
- Mixing UI logic into DSP classes.
- Overcomplicating voice stealing too early.

---

## Suggested Week 1 progression

Day 1

- Single oscillator + ADSR + note handling

Day 2

- Add saw or square waveform and compare aliasing behavior

Day 3

- Add simple filter (start with stable low-pass)

Day 4

- Add one LFO and route to pitch or filter cutoff

Day 5

- Build a basic modulation matrix abstraction

Day 6

- Add parameter smoothing everywhere needed

Day 7

- Refactor for modularity and document signal flow

---

## What comes next after this guide

Once the minimal voice is stable, the next learning milestone is:

- Band-limited oscillators
- Better filter design and stability
- Modulation routing design patterns
- Preset/state serialization
- CPU profiling and optimization

If you want, the next document can be a method-by-method rubric for each class with expected behavior and quick debug probes (what to print/plot/listen for at each stage).
