# 3D Audio Example

##Usage

**Makefile**
Run `make`
Run the executable like `./bin/immersitech_example.exe ../../audio_files/WaroftheWorlds1.wav ../../audio_files/WaroftheWorlds2.wav`

**Visual Studio**
Add `$(ProjectDir)../../audio_files/WaroftheWorlds1.wav $(ProjectDir)../../audio_files/WaroftheWorlds2.wav` as the Command Arguments in the Debugging configuration for the project
Build the project

Open `output_file_1.wav` and `output_file_2.wav` to listen to the stereo tracks created by the Immersitech library
On Mac you can run these in parallel with `afplay output_file_1.wav & afplay output_file_2.wav`