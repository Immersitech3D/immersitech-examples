#include "immersitech_clearvoice.h"
#include "audiofile.h"

#include <stdio.h>

int main(int argc, const char* argv[])
{
    if (argc < 7)
    {
        std::cout << "Usage: \nclearvoice_demo.exe <licensefile> <input.wav> <output.wav> <anc_mix> <agc_enable> <aeq_enable>" << std::endl;
        return 1;
    }

    const char* license_filepath = argv[1];
    const char* input_audio_file = argv[2];
    const char* output_audio_file = argv[3];
    int anc_mix = atoi(argv[4]);
	int aeq_enabled = atoi(argv[5]);
	int agc_enabled = atoi(argv[6]);

    // Load input file
    AudioFile<float> input_file;
    bool loadedOK = input_file.load(input_audio_file);
    if (loadedOK == false) {
        std::cout << "Failed to load input file " << input_audio_file << std::endl;
        return 1;
    }
    int sample_rate = input_file.getSampleRate();
    int num_channels = input_file.getNumChannels();
    if (num_channels > 1) {
        std::cout << "Sorry, only mono files are supported with the ClearVoice library." << std::endl;
        return 1;
    }
    int max_samples = input_file.getNumSamplesPerChannel();
    int buffer_size = sample_rate / 100; // ClearVoice always processes using 10ms buffers

    // Create output file
    AudioFile<float> output_file;
    output_file.setNumSamplesPerChannel(max_samples);

    // Initialize Immersitech ClearVoice
    imm_clearvoice speech_enhancer = imm_clearvoice(license_filepath, sample_rate, aeq_enabled, agc_enabled, anc_mix);

    // Process audio file one buffer at a time
    int s = 0;
    while(s < max_samples) {
        speech_enhancer.process(&input_file.samples[0][s], &output_file.samples[0][s]);
        s = s + buffer_size;
    }

    // Write the output file
    output_file.setBitDepth(16);
    output_file.save(output_audio_file, AudioFileFormat::Wave);
    
    return 0;
}
