### To compile
Update the paths in CMakeLists.txt to reflect the true location of the ClearVoice Library/header and the IPP Libraries/headers (if on Intel). Then:
```
cmake -S . -B build
cmake --build build 
```

### To run
Navigate to the clearvoice_demo executable (in the build folder) and run 
```
./clearvoice_demo <path/to/license/file> <input.wav> <output.wav> 100 1 1
```