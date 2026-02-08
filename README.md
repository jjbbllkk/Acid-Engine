# Acid Engine

A TB-303 acid synth emulation for VCV Rack 2.

## Overview

Acid Engine brings the iconic sound of the Roland TB-303 to VCV Rack, featuring authentic filter squelch, accent behavior, and slide. The module offers three operational modes ranging from tame to wild.

## Installation

1.  Go to the [releases page](https://github.com/jjbbllkk/Acid-Engine/releases).
2.  Download the `.vcvplugin` file for your operating system. For example, `AcidEngine-2.0.0-mac-arm64.vcvplugin` for Apple Silicon Macs.
3.  Place the downloaded `.vcvplugin` file into the VCV Rack `plugins` folder for your operating system:
    -   **macOS:** `~/Documents/Rack2/plugins/`
    -   **Windows:** `C:\Users\<Your Username>\Documents\Rack2\plugins\`
    -   **Linux:** `~/.Rack2/plugins/`
4.  Restart VCV Rack. Rack will automatically unzip the `.vcvplugin` file and delete it after installation.

## Controls

### Knobs

| Knob | Description |
|------|-------------|
| **Tuning** | Master tuning offset (+/- 12 semitones) |
| **Cutoff** | Filter cutoff frequency |
| **Resonance** | Filter resonance amount |
| **Decay** | Filter envelope decay time |
| **EnvMod** | Filter envelope modulation depth |
| **Slide** | Portamento/glide time between notes |
| **Accent** | Intensity of accent effect (squelch + volume boost) |

### Switches

| Switch | Options |
|--------|---------|
| **Waveform** | Saw / Blend / Square |
| **Mode** | Baby Fish (tame) / Momma Fish (standard) / Devil Fish (extended ranges) |

### Inputs

| Input | Description |
|-------|-------------|
| **V/Oct** | Pitch CV (1V/octave) |
| **Trig** | Gate/trigger input |
| **Accent** | Gate input (>2.5V triggers accent on that note) |
| **Cutoff CV** | Filter cutoff modulation |
| **Res CV** | Resonance modulation |
| **Decay CV** | Decay time modulation |
| **Slide CV** | Slide amount modulation |
| **EnvMod CV** | Envelope mod depth modulation |

### Outputs

| Output | Description |
|--------|-------------|
| **Out L / Out R** | Audio output (mono, duplicated to both) |

## Accent Behavior

The accent works like a real 303:

- **Accent knob** controls the intensity (how much squelch and volume boost)
- **Accent CV** acts as a gate - send >2.5V to accent that note
- Accented notes get: faster filter envelope (5x shorter decay), extra cutoff modulation, and amplitude boost

## Mode Differences

| Parameter | Baby Fish | Momma Fish | Devil Fish |
|-----------|-----------|------------|------------|
| Cutoff | 200-2000 Hz | 100-4000 Hz | 20-8000 Hz |
| Resonance | 0-50% | 0-80% | 0-100% |
| Decay | 200-1000 ms | 200-2000 ms | 30-3000 ms |
| EnvMod | 0-50% | 0-80% | 0-100% |
| Accent | 0-25% | 0-50% | 0-100% |

## Credits

- **Open303 DSP engine**: [Robin Schmidt](http://www.rs-met.com/) (from the `rosic` library)
- **Noise Engineering Versio Platform Port**: [abluenautilus](https://github.com/abluenautilus)
- **VCV Rack Port**: [Vulpes79](https://github.com/jjbbllkk)

## License

MIT License
