# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Build the plugin (from plugin directory)
make

# Clean build artifacts
make clean

# Install to Rack plugins folder
make install
```

The Makefile expects the Rack SDK at `../..` (two directories up). Override with `RACK_DIR=/path/to/sdk make`.

## Architecture

This is a VCV Rack 2.x plugin implementing a TB-303 acid synth emulation called "Acidus Versio".

### Key Files

- `src/plugin.cpp` / `src/plugin.hpp` - Plugin registration and global instance
- `src/AcidusVersio.cpp` - Main module: parameters, inputs/outputs, audio processing via `Open303` engine
- `src/open303/` - The Open303 synthesizer engine (rosic library), providing TB-303 emulation

### Module Structure

The `AcidusVersio` module wraps the `rosic::Open303` synth engine:
- **Inputs**: V/Oct pitch, Gate, Accent CV
- **Outputs**: Audio out (scaled to 5V)
- **Params**: Decay, Cutoff, Resonance, EnvMod, Accent, Slide

### Open303 Engine Components

The synth engine in `src/open303/` follows a signal flow:
- `BlendOscillator` - Saw/square wave blend
- `TeeBeeFilter` - 303-style resonant lowpass filter
- `AnalogEnvelope` / `DecayEnvelope` - Amplitude and filter envelopes
- `LeakyIntegrator` - Pitch slew for slide/portamento
- Various highpass/allpass/notch filters for authentic 303 tone shaping

### Panel

The SVG panel is at `res/AcidusVersio.svg`. Widget positions in `AcidusVersioWidget` use `mm2px()` coordinates.
