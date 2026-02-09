# vulpes79 VCV Rack Module Design Language

This document captures the visual design system used by the AcidEngine module. Use it as a reference when building or restyling other vulpes79 modules to ensure visual consistency.

## Brand

- **Brand name**: vulpes79
- **Brand label position**: Bottom of panel, centered, in the output section dark background
- **Brand font**: JetBrains Mono Regular, small size (~2.1px in SVG units), fill `#b3b3b3`

## Typography

All text uses **JetBrains Mono** exclusively.

| Usage | Weight | Size (SVG mm) | Fill |
|-------|--------|---------------|------|
| Module title | Ultra-Bold (800) | 3.53 | `#1a1a1a` |
| Knob/control labels | Bold (700) | 2.12 | `#1a1a1a` |
| CV input labels | Semi-Bold (600) | 1.41 | `#b3b3b3` |
| Output labels | Semi-Bold (600) | 1.76 | `#b3b3b3` |
| Brand (vulpes79) | Regular (400) | ~2.1 (scaled) | `#b3b3b3` |

Text is converted to `<path>` elements in the rendered layer for portability. An editable text layer (`display:none`) is kept for future editing in Inkscape.

## Color Palette

### Backgrounds
| Element | Color | Notes |
|---------|-------|-------|
| Main panel | `#e6e6e6` | Light warm gray |
| Section grouping rects | `#dcdcdc` | Subtle darker gray, rx=2 rounded corners |
| CV input area | `#222222` | Dark band |
| Output area | `#1a1a1a` | Darkest band |
| VU meter bar | `#e2e3db` | Muted warm gray-green tint |

### Accent Color: Yellow
| Element | Color |
|---------|-------|
| CUTOFF halo ring | `#ffff00` (stroke, 0.4 width) |
| CV section divider line | `#ffff00` (stroke, 0.5 width) |
| VU meter dot fills | `#ffff00` |

**Note**: The accent color is **yellow** (`#ffff00`), not green. All decorative accent elements use this color.

### Text & Outlines
| Element | Color |
|---------|-------|
| Control labels (dark bg) | `#1a1a1a` |
| CV/output labels (light on dark) | `#b3b3b3` |
| Knob outline circles | `#bbbbbb` stroke, 0.3 width, no fill |
| CV jack outline circles | `#444444` stroke, 0.3 width, no fill |
| Switch outlines | `#bbbbbb` stroke, 0.3 width, no fill |
| Icon strokes (waveform) | `#999999` |
| Icon fills (fish) | `#999999` |

## Knob Hierarchy (C++ Widget Types)

Three tiers of Rogan knobs, all in **white/black** (not green):

| Tier | Widget | Approx Radius | Used For |
|------|--------|---------------|----------|
| Hero | `Rogan3PSWhite` | ~8.5mm | Primary parameter (e.g. CUTOFF) |
| Secondary | `Rogan1PSWhite` | ~5.0mm | Important parameters (e.g. DECAY, ENVMOD) |
| Tertiary | `Rogan1PWhite` | ~4.0mm | Supporting parameters (e.g. TUNE, RES, SLIDE, ACCENT) |

The hero knob should have a decorative **yellow halo ring** (SVG circle, `stroke:#ffff00`, `stroke-width:0.4`, `fill:none`, radius ~11mm) to draw the eye.

## VU Meter

- 3 LEDs, horizontal row, centered in the VU bar
- C++ type: `MediumLight<GreenLight>`
- SVG backing dots: circles with `fill:#ffff00`, `stroke:#555555`, `stroke-width:0.3`, `r=1.6`
- Bar background: rounded rect (`rx=1`), fill `#e2e3db`

## Other Widgets

| Widget | VCV Type | Notes |
|--------|----------|-------|
| 3-way switches | `CKSSThree` | For waveform, mode selection |
| Trigger button | `VCVButton` | Manual trigger |
| CV jacks | `PJ301MPort` | Standard for both inputs and outputs |
| Screws | `ScrewSilver` | Four corners |

## Panel Layout Structure

The panel is 60.96mm wide (4HP) x 128.5mm tall, divided into visual zones from top to bottom:

1. **Title bar** (~y=0-8): Module name "ACID ENGINE" in Ultra-Bold, flanked by decorative chevron/arrow graphics (`#221f1f` fill)
2. **VU meter** (~y=9-14): Horizontal bar with 3 LED dots
3. **Filter section** (~y=16-44): Grouping rect background. Contains TUNE (left), RES (right), and CUTOFF (center, hero knob with yellow halo)
4. **Modulation section** (~y=41-55): Grouping rect background. Contains DECAY (left), ENVMOD (right) in same columns as TUNE/RES
5. **Performance section** (~y=53-83): Grouping rect background. Contains WAVE switch + waveform icons, SLIDE knob, MODE switch + fish icons, ACCENT knob, TRIG button
6. **Yellow accent line** (~y=83.3): Full-width horizontal divider, `stroke:#ffff00`, `stroke-width:0.5`
7. **CV input area** (~y=83.7-110): Dark background (`#222222`). Two rows of 4 jacks each with light gray labels
8. **Output area** (~y=110-128.5): Darkest background (`#1a1a1a`). OUT L and OUT R jacks with labels and brand name

Section grouping rects overlap slightly at edges for natural visual transitions.

## Acid Smiley

A detailed acid house smiley face graphic lives in its own sublayer (`inkscape:label="acid smiley"`) within layer1. It is positioned in the performance section area (transform places it around x=42, y=54, scaled very small via `matrix(0.00334,...)`).

The smiley consists of multiple `<path>` elements:
- **Yellow fill** (`#f7e71d`): The face/head shape and melting hair detail
- **Dark fill** (`#292a2c`): Face outline, eyes (oval/pill shapes), and smile

This is a signature brand element and should be preserved or adapted for other modules. It provides character and ties into the acid house aesthetic.

## Decorative Elements

### Title Chevrons
Two mirrored arrow/chevron shapes flank the title at the top of the panel. They use `fill:#221f1f` and add an industrial/electronic feel. Left chevron points right, right chevron points left.

### Waveform Icons
Small hand-drawn-style waveform graphics (saw, blend/trapezoid, square) next to the WAVE switch. Stroke-only, `#999999`, `stroke-width:0.3`.

### Fish Icons
Three fish icons next to the MODE switch representing Baby Fish, Momma Fish, and Devil Fish modes. Fill `#999999` with `#e6e6e6` eyes. Devil fish has small horn strokes.

## SVG Layer Structure

| Layer | Name | Visibility | Contents |
|-------|------|------------|----------|
| layer1 | Layer 1 | visible | All rendered graphics: backgrounds, outlines, icons, decorative elements, acid smiley |
| layer2 | text-editable | hidden | Editable text elements (for Inkscape editing workflow) |
| g37 | text-editable copy | visible | Path-converted text labels (what actually renders) |

## Key Design Principles

1. **Monochrome base with yellow accents**: The panel is primarily grayscale. Yellow is used sparingly for accents (halo, divider line, VU dots) to create focal points
2. **Visual hierarchy through knob size**: The most important parameter gets the biggest knob and a decorative ring
3. **Section grouping**: Subtle background rects in slightly darker gray separate control groups without harsh borders
4. **Dark-to-light gradient**: The panel flows from light (controls) to dark (CV/outputs), creating natural visual separation
5. **Consistent column alignment**: Parameters that share a column (TUNE/DECAY, RES/ENVMOD) are vertically aligned at the same x-coordinate
6. **Font consistency**: JetBrains Mono everywhere, no exceptions
7. **Personality through icons**: Custom waveform, fish, smiley, and chevron graphics add character while remaining subtle
