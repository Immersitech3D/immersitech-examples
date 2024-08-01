#include "immersitech_clearvoice.h"
#include "audiofile.h"

#include <stdio.h>

int main(int argc, const char* argv[])
{
    if (argc < 4)
    {
        std::cout << "Usage: \nclearvoice_demo.exe <licensefile> <input.wav> <output.wav> " << std::endl;
        return 1;
    }

    const char* license_filepath = argv[1];
    const char* input_audio_file = argv[2];
    const char* output_audio_file = argv[3];

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
    output_file.setSampleRate(sample_rate);

    // Create necessary variables
    imm_cv_config config;
    imm_error_code error_code;
    imm_cv_output_metadata metadata;
    imm_cv_handle handle;

    // Configure Immersitech ClearVoice
    config = imm_cv_get_default_config();
    config.input_sample_rate = sample_rate;
    config.output_sample_rate = sample_rate;

    // Initialize Immersitech ClearVoice
    handle = imm_cv_init_from_file(license_filepath, config, &error_code);
    if (error_code != IMM_ERROR_NONE) {
        return 1;
    }

    // Process audio file one buffer at a time
    int s = 0;
    while(s < max_samples) {
        imm_cv_process(handle, &input_file.samples[0][s], &output_file.samples[0][s], &metadata);
        s = s + buffer_size;
    }

    // Write the output file
    output_file.setBitDepth(16);
    output_file.save(output_audio_file, AudioFileFormat::Wave);
    
    return 0;
}
