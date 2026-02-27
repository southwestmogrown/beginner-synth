# Plugin Type Conversion Checklist (for `beginner-synth`)

This checklist shows the minimum changes needed to repurpose the current scaffold.

## 1) Instrument (Synth) mode

In `CMakeLists.txt`:

- `IS_SYNTH TRUE`
- `NEEDS_MIDI_INPUT TRUE`
- `IS_MIDI_EFFECT FALSE`

In processor code:

- Generate audio from MIDI notes.
- No mandatory audio input path.

Use when:

- Building synths, samplers, drum synths.

---

## 2) Audio Effect mode (delay, reverb, distortion)

In `CMakeLists.txt`:

- `IS_SYNTH FALSE`
- `NEEDS_MIDI_INPUT FALSE` (or TRUE only if effect reacts to MIDI)
- `IS_MIDI_EFFECT FALSE`

In processor code:

- Ensure constructor has both input and output buses.
- Process incoming audio in `processBlock`.
- Start with pass-through, then add DSP (e.g., delay line).

Use when:

- Delay, chorus, compressor, EQ, saturator.

---

## 3) MIDI Effect mode

In `CMakeLists.txt`:

- `IS_SYNTH FALSE`
- `IS_MIDI_EFFECT TRUE`
- `NEEDS_MIDI_INPUT TRUE`
- Usually no audio generation/processing.

In processor code:

- Transform MIDI messages (transpose, chordify, arpeggiate, etc.).

Use when:

- MIDI tools and generators.

---

## 4) Bus/layout sanity checks

- Synths: output bus is required; input bus is optional.
- Audio effects: input and output buses should match.
- Keep `isBusesLayoutSupported` aligned with plugin type.

---

## 5) Quick validation after changing type

- Re-run configure:
  - `cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug`
- Rebuild:
  - `cmake --build build`
- Re-scan plugin in your host.
- Verify behavior:
  - Synth: MIDI triggers sound.
  - Effect: audio input is processed and audible.
  - MIDI effect: MIDI output changes as expected.
