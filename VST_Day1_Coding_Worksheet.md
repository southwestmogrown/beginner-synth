# VST Day 1 Worksheet (Absolute Beginner C++ + JUCE Setup)

Date: February 27, 2026

Purpose: get from zero C++ plugin setup to a playable VST3 tone on Linux.

Timebox target: 3 to 6 hours (first-time setup is the slow part).

---

## What this version changes

This worksheet assumes you have never written C++ before, so it includes:

- Linux environment setup
- JUCE dependency setup
- A minimal CMake project path
- Beginner-friendly coding checkpoints

---

## Step A: Install tools (Linux)

Use the command set that matches your distro.

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build git pkg-config \
    libasound2-dev libjack-jackd2-dev libx11-dev libxext-dev \
    libxrandr-dev libxinerama-dev libxcursor-dev libfreetype6-dev \
    libwebkit2gtk-4.1-dev libglu1-mesa-dev mesa-common-dev
```

### Fedora

```bash
sudo dnf groupinstall -y "Development Tools"
sudo dnf install -y cmake ninja-build git alsa-lib-devel jack-audio-connection-kit-devel \
    libX11-devel libXext-devel libXrandr-devel libXinerama-devel libXcursor-devel \
    freetype-devel webkit2gtk4.1-devel mesa-libGLU-devel
```

### Arch

```bash
sudo pacman -Syu --noconfirm base-devel cmake ninja git alsa-lib jack2 \
    libx11 libxext libxrandr libxinerama libxcursor freetype2 webkit2gtk glu
```

Pass criteria:

- `cmake --version` works
- `g++ --version` or `clang++ --version` works

---

## Step B: Install a test host and scanner tools

Install at least one VST3 host so you can load your plugin.

Options:

- REAPER (recommended for learning)
- Ardour
- Carla (good for quick plugin validation)

Pass criteria:

- Host opens and can scan VST3 plugins.

---

## Step C: Create project and pull JUCE

From your workspace root:

```bash
git clone https://github.com/juce-framework/JUCE.git
mkdir -p beginner-synth && cd beginner-synth
```

Create a `CMakeLists.txt` that points to `../JUCE` and defines one JUCE Audio Plug-In target.

Minimum target intent:

- Plugin formats: VST3
- Type: Synth (accept MIDI input, produce audio output)
- Sources: `PluginProcessor.cpp/.h`, `PluginEditor.cpp/.h`

Pass criteria:

- CMake configure succeeds.

Debug probe:

- If configure fails, solve includes/dependency errors before writing DSP.

---

## Step D: Build and run the empty plugin

In project directory:

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Pass criteria:

- Build completes.
- A `.vst3` bundle/artifact is produced.
- Host discovers the plugin.

If host does not find it:

- Add plugin path in host settings.
- Re-scan plugins.

---

## C++ mini-primer (just enough for Day 1)

You only need these concepts today:

- `class`: groups data + functions.
- Member variables hold state between calls.
- Methods like `prepare`, `noteOn`, `renderSample` are functions on objects.
- `float` is common for audio sample math.
- `for` loop processes samples one-by-one.

Mental model:

- Each voice is an object with its own oscillator phase + envelope level.

---

## Day 1 coding target

Build one voice that does:

- MIDI note-on -> sine tone starts
- MIDI note-off -> envelope releases cleanly
- No obvious clicks

Signal flow per sample:

`sample = oscillator * envelope * gain`

---

## Suggested file structure

- `Oscillator.h/.cpp`
- `EnvelopeADSR.h/.cpp`
- `Voice.h/.cpp`
- `PluginProcessor.h/.cpp`

If JUCE generated slightly different files, keep those and add your classes beside them.

---

## Ordered implementation checklist (beginner mode)

### Step 1: compile-first skeleton

Task:

- Create empty classes and method signatures.
- Wire into processor without real DSP.

Pass criteria:

- Plugin still builds and loads.

### Step 2: oscillator only

Implement in order:

1. `prepare(sampleRate)` stores sample rate and resets phase.
2. `setFrequency(hz)` sets `phaseIncrement = hz / sampleRate` safely.
3. `renderSample()` computes sine and wraps phase into `[0, 1)`.

Pass criteria:

- Fixed note produces continuous tone (even before envelope).

Debug probe:

- Temporary log frequency and increment when note starts.

### Step 3: envelope ADSR

Implement in order:

1. Idle/attack/decay/sustain/release state enum.
2. `noteOn()` enters attack.
3. `noteOff()` enters release.
4. `renderSample()` updates one step per sample.
5. `isActive()` true unless idle.

Pass criteria:

- Envelope ramps up and down (no hard jumps).

Debug probe:

- Print first 20 envelope samples on note-on and note-off tests.

### Step 4: voice class

Implement in order:

1. `startNote(midiNote, velocity)` sets frequency + gain + noteOn.
2. `stopNote(allowTailOff)` does release or hard stop.
3. `render(...)` sums `osc * env * gain` into output.

Pass criteria:

- A single key press plays and releases cleanly.

### Step 5: MIDI routing in processor

In `processBlock`:

1. Clear output buffer.
2. Read MIDI events.
3. Dispatch note-on/note-off to voice(s).
4. Render active voice samples.

Pass criteria:

- Keyboard input from host triggers sound correctly.

### Step 6: add smoothing and polyphony

Add:

- master gain smoothing (5 to 20 ms)
- fixed voice count (for example 8)
- simple allocator: first inactive, else steal one deterministic voice

Pass criteria:

- Two-note overlap works.
- Fast repeated notes do not stick.

---

## Real-time safety checklist (must keep)

- No heap allocations in `processBlock`.
- No locks/mutex waits in audio thread.
- No disk/network I/O in audio thread.
- Re-run `prepare` logic correctly when sample rate changes.

---

## Final acceptance checklist (Day 1)

- [ ] Toolchain installed and verified.
- [ ] JUCE pulled and CMake config succeeds.
- [ ] Plugin builds and is found by host.
- [ ] MIDI note-on starts tone.
- [ ] MIDI note-off releases smoothly.
- [ ] No obvious clicks.
- [ ] Basic polyphony works.
- [ ] No stuck notes during rapid play.

---

## Fast troubleshooting map

- Build fails at CMake configure:
  - missing distro packages (install dev deps first)
- Plugin builds but host cannot find it:
  - incorrect plugin scan path or no re-scan
- Click at note boundaries:
  - envelope times too short or gain jumps without smoothing
- Wrong pitch:
  - MIDI note-to-frequency conversion or sample rate handling bug
- Stuck notes:
  - note-off event not routed to the matching active voice

---

## Next step after Day 1

After this passes, move to Day 2:

- add saw/square waveform
- learn aliasing by ear
- introduce first anti-aliasing strategy
