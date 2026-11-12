# VocalCompressor

A two-stage vocal compressor plugin built with C++ and JUCE: a fast FET compressor (**FET-76**) and an optical leveler (**OPT-2A**) in series, the classic FET-into-opto vocal chain. Both stages are inspired by well-known classic hardware units. The order can be reversed, and each stage has its own bypass and dry/wet mix.

Formats: VST3, AU and Standalone.

## Features

### FET-76 (FET compressor)
- Input and Output knobs with tapered gain laws. Input drives the signal into a fixed threshold, as on the hardware it is modelled on.
- Attack from 20 µs to 800 µs and release from 50 ms to 1.1 s.
- Ratio buttons: 4, 8, 12, 20 and ALL.
- ALL mode: program-dependent ratio, a threshold that drops as gain reduction builds, and an attack that speeds up with gain reduction.
- Saturation that follows the amount of gain reduction.
- Mix knob for parallel compression.

### OPT-2A (optical leveler)
- Gain and Peak Reduction knobs. Peak Reduction sets the threshold, so the knob at zero means no compression.
- COMP / LIMIT switch: soft-knee 3:1 or firmer 10:1.
- Program-dependent release that moves from fast to slow as the signal gets louder, like an optical cell.
- Gentle tube-style saturation.
- Mix knob.

### Signal chain
- Serial chain with a reversible order. Switching the order crossfades between the old and the new order over 20 ms, so it never drops to the dry signal or restarts the envelopes.
- Smoothed bypass (15 ms dry/wet ramp) on each stage and on the whole plugin. The plugin bypass is also exposed to the host as its bypass parameter.
- Parameters are smoothed and applied in 32-sample sub-blocks, independent of the host buffer size.

### Oversampling
- The whole chain runs at 2x the host sample rate, including both compressors, their mix and every bypass crossfade.
- The oversampler is a custom polyphase IIR half-band filter written in plain C++ (`Source/Utilities/AADspTools.h`), with about 99 dB of stopband attenuation.
- Latency: 3 samples at the host rate, reported to the host for delay compensation and constant whether or not the plugin is bypassed.

### Interface
- Hardware-style rack panels with image-based knobs, printed scales and LED gain reduction meters.
- The panels swap places when the processing order is reversed.
- Preset browser with factory and user presets, save dialog, and undo / redo of parameter changes and preset loads.
- Resizable UI with five sizes (X-Small to X-Large). The chosen size is remembered as the default for new instances.

## Building

Requirements: CMake 3.22 or newer and a C++20 compiler (Xcode on macOS, Visual Studio 2022 on Windows).

JUCE is found in this order:
1. `-DJUCE_PATH=/path/to/JUCE` (an existing JUCE checkout)
2. a `JUCE` folder in the project root
3. otherwise JUCE 8.0.11 is downloaded automatically with FetchContent

macOS (universal binary, arm64 + x86_64):

```bash
cmake --preset mac_ninja_multi
cmake --build --preset mac_ninja_multi_release
```

Windows:

```bash
cmake --preset win_ninja_multi
cmake --build --preset win_ninja_multi_release
```

The plugins are copied to the system plugin folders after a successful build.

## Project structure

```
Source/
  PluginProcessor.*      Parameters, signal chain, oversampling, order crossfade, state
  PluginEditor.*         Editor layout, popups and dimming
  DSP/                   FET1176Compressor and LA2ACompressor
  GUI/
    Panels/              Header, FET-76 and OPT-2A panels
    Components/          Image knobs, ratio buttons, gain reduction meter, preset box
    PopupPanels/         Preset browser, save dialog, UI size menu
    LAF/                 LookAndFeel classes
  Utilities/             Preset manager, undo manager, UI resizing, DSP and UI helpers
  Assets/                Images, icons, fonts and factory presets (embedded as binary data)
```
