# Pre-Dripp

Pre-Dripp is a JUCE/C++ audio plugin project focused on a tactile stem-mixing workflow: circular stem controls, fast positioning presets, and a compact channel strip with EQ, compression, saturation, reverb, stereo width, and output protection.

## Target formats

- VST3
- Standalone app for development/testing
- AU can be enabled on macOS

## Architecture

- JUCE + CMake
- AudioProcessorValueTreeState parameter/state model
- Modular per-stem DSP chain
- Custom editor designed for a dark, high-contrast production workflow
- GitHub Actions verification

The first implementation is developed on a feature branch and reviewed through a pull request.
