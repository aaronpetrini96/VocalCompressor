# VocalCompressor

A two-stage vocal compressor plugin built with C++ and JUCE: a fast FET compressor (**FET-76**) into an optical leveler (**OPT-2A**), the classic vocal chain. Both stages are inspired by well-known classic hardware units.

## Formats

* macOS: AU and VST3
* Windows: VST3

On Windows, unzip and copy `VocalCompressor.vst3` to `C:\Program Files\Common Files\VST3`, then rescan plugins in your DAW.

## Features

### FET-76
- Input and Output knobs: Input drives the signal into the compressor, Output sets the level.
- Fast attack and release controls.
- Ratio buttons: 4, 8, 12, 20 and ALL, the aggressive all-buttons-in mode.
- Harmonic saturation that gets stronger at higher ratios.
- Mix knob for parallel compression.

### OPT-2A
- Gain and Peak Reduction knobs: Peak Reduction sets how much the signal is levelled.
- COMP / LIMIT switch.
- Program-dependent release, like an optical cell.
- Gentle tube-style saturation.
- Mix knob.

### Chain
- Reversible order: run FET-76 into OPT-2A, or the other way round, with a click-free switch.
- Bypass for each stage and for the whole plugin.
- 2x oversampling on the whole chain, with latency reported to the host.

### Interface
- Hardware-style rack panels with LED gain reduction meters.
- The panels swap places when the order is reversed.
- Preset browser with factory and user presets, plus undo / redo.
- Resizable UI with five sizes; the chosen size is remembered.

## Building

Requirements: CMake 3.22 or newer, Ninja, and a C++20 compiler (Xcode on macOS, Visual Studio 2022 on Windows).

JUCE 8.0.11 is downloaded automatically. To use an existing checkout, pass `-DJUCE_PATH=/path/to/JUCE` or place it in a `JUCE` folder in the project root.

macOS (universal binary):

```bash
cmake --preset mac_ninja_multi
cmake --build --preset mac_ninja_multi_release
```

Windows (run from an administrator shell so the plugin can be copied to the system VST3 folder):

```bash
cmake --preset win_ninja_multi
cmake --build --preset win_ninja_multi_release
```

A Standalone app is also built, for development and debugging only.

## Project structure

```
Source/
  PluginProcessor.*      Parameters, signal chain and state
  PluginEditor.*         Editor layout and popups
  DSP/                   FET-76 and OPT-2A compressors
  GUI/                   Panels, components, popups and LookAndFeel classes
  Utilities/             Presets, undo, UI resizing and helpers
  Assets/                Images, icons, fonts and factory presets
```
