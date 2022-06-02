# Immersitech Engage™ Core SDK

The Immersitech SDK is a C / C++ library that allow you to easily augment your existing audio applications with several audio effects, including noise cancellation, 3D spatial audio, and voice clarity effects. Provide the library with an input audio buffer and the library will output an audio buffer with the enhanced audio. See our [developer](https://immersitech.io/developer/) page for more information.

# License Key
Please reach out to us about starting a [trial](https://immersitech.io/trialsdk/) of the Immersitech Engage™ Core SDK now.

# Release Binaries

This repository contains the latest release of the Immersitech Engage SDK binaries. It is recommended that if are getting started with the SDK you download
one of the tar files for Mac or Windows. Some of the release names have `-full` or `no-spatial-audio` appended. Choose the version which is right for your use case.

If you are looking to get started with our [Public Freeswitch Adapter](https://github.com/Immersitech3D/public_freeswitch_adapter) then you will want to download either the Debian or Ubuntu release files.

# Immersitech SDK Examples

- Clone this repository

**Windows**
- Run `mkdir lib`
- Drop your .dll, .lib, or .dylib files into the `lib` folder

**Mac**
- Run `mkdir /usr/local/lib/immersitech` as an administrator
- Place your .dylib file into `/usr/local/lib/immersitech`

- Run `mkdir license`
- Place your license key in the `license` folder
- Build each individual example using the provided Visual Studio project or makefile.

Example WAV files are provided [here](https://github.com/Immersitech3D/immersitech-engage-examples/tree/master/audio_files)

