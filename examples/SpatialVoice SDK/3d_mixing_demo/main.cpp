#include "immersitech.h"
#include "immersitech_logger.h"

#include "audiofile.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define OUTPUT_SAMPLE_RATE (48000)
#define OUTPUT_NUM_FRAMES (480)

imm_participant_configuration participant_config;
imm_error_code error_code;

/*

This command line tool takes an input wav file and processes it through the SDK. The processed
audio is stored as a wav file.

IMPORTANT:
The input wav files MUST be mono and have a sample rate of 48kHz.

SYNTAX:
3d_mixing_demo.exe <input_1.wav> <input_2.wav> ... <input_N.wav>

*/
int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: \n3d_mixing_demo.exe <input_1.wav> <input_2.wav> ... <input_N.wav> \nThe input wav files MUST be mono and have a sample rate of 48kHz." << std::endl;
        return 1;
    }

    // We will keep track of how many participants there are as the number of files input on the command line
	int number_participants = argc - 1;

	// We can save information about each participant to reference them later
	int i;
	int* participant_sampling_rates		= (int*)malloc(number_participants * sizeof(int));	// The sampling rate of each input file
	int* participant_num_channels		= (int*)malloc(number_participants * sizeof(int));	// The number of channels in each input file
	int* participant_num_input_frames	= (int*)malloc(number_participants * sizeof(int));	// How many frames this participant will need to input each buffer
    
    imm_enable_logging(true);
    imm_set_log_level(imm_log_level::IMM_LOG_DEBUG);

    /* Initialize IMM library */
    imm_library_configuration config;
    config.interleaved = false;
    config.output_number_channels = 2;
    config.output_number_frames = OUTPUT_NUM_FRAMES;
    config.output_sampling_rate = OUTPUT_SAMPLE_RATE;
    config.spatial_quality = 3;

    imm_handle imm_instance = imm_initialize_library("Immersitech_Engineering_sound_manager_license_key.dat", NULL, NULL, config, &error_code);
    if (error_code != IMM_ERROR_NONE)
    {
        /* Error */
        std::cout << "imm_initialize_library failed with error code " << error_code <<std::endl;
    }
    
    /* Create room */
    int room_id = 0;
    error_code = imm_create_room(imm_instance, room_id);
    if (error_code != IMM_ERROR_NONE) {
        /* Error */
        std::cout << "imm_create_room failed with error code " << error_code <<std::endl;
    }

    /* Load input files */
    int maxNumSamples = 0;
    AudioFile<float>* inputFiles = new AudioFile<float>[number_participants];
    for (int i = 0; i < number_participants; i++) {
        bool loadedOK = inputFiles[i].load(argv[i+1]);
        if (loadedOK == false) {
            /* Error */
            std::cout << "Failed to load input file " << argv[i+1] << std::endl;
        }
        participant_sampling_rates[i] = inputFiles[i].getSampleRate();
        participant_num_channels[i] = inputFiles[i].getNumChannels();
        participant_num_input_frames[i] = (OUTPUT_NUM_FRAMES * participant_sampling_rates[i]) / OUTPUT_SAMPLE_RATE;

        maxNumSamples = std::max(maxNumSamples, inputFiles[i].getNumSamplesPerChannel());
    }

    /* Create output files */
    AudioFile<float>* outputFiles = new AudioFile<float>[number_participants];
    for (int i = 0; i < number_participants; i++) {
        outputFiles[i].setSampleRate(OUTPUT_SAMPLE_RATE);
        outputFiles[i].setNumChannels(2);
        outputFiles[i].setNumSamplesPerChannel(maxNumSamples + OUTPUT_SAMPLE_RATE);
    }
    
    /* Add each participant to the room */
    for (int i = 0; i < number_participants; i++) {
        participant_config.input_number_channels = participant_num_channels[i];
        participant_config.input_sampling_rate = participant_sampling_rates[i];
        participant_config.type = IMM_PARTICIPANT_REGULAR;
        error_code = imm_add_participant(imm_instance, room_id, i, "participant", participant_config);
        if (error_code != IMM_ERROR_NONE) {
            /* Error */
            std::cout << "imm_add_participant failed with error code " << error_code <<std::endl;
        }
    }

    /* Ensure 3D mixing is enabled for all participants */
    imm_set_all_participants_state(imm_instance, room_id, IMM_CONTROL_MIXING_3D_ENABLE, 1);
    imm_set_all_participants_state(imm_instance, room_id, IMM_CONTROL_MIXING_3D_ATTENUATION, 6);
	imm_set_all_participants_state(imm_instance, room_id, IMM_CONTROL_MIXING_3D_MAX_DISTANCE, 300);

    /* Set each participant's location in the room */
	imm_position position = { 0,0,0 };
	imm_heading heading = { 0,0 };
	for (i = 0; i < number_participants; i++) {
		/* Randomize the positions of the participants
		   Also be aware of the heading for each participant. 
           The direction they are facing will change their 
           perspective of the 3d scene. */
		if (i % 2) {
			position.x = i * 20;
			position.z = i * 30;
		}
		else {
			position.x = i * -50;
			position.z = i * 50;
			heading.azimuth_heading = 135;
		}
		imm_set_participant_position(imm_instance, room_id, i, position, heading);
	}

    float* input_buffer = new float[2048];
    float* output_buffer = new float[2048];
    int s = 0;
    while (1) {
        /* In this example, we will just end when the first file is finished */
        if (s*participant_num_input_frames[0] > inputFiles[0].getNumSamplesPerChannel()){
            break;
        }

        /* Input audio for each participant */
        for (int i = 0; i < number_participants; i++) {
            if (participant_num_channels[i] == 1) {
                memcpy(input_buffer, &inputFiles[i].samples[0][s * participant_num_input_frames[i]], participant_num_input_frames[i] * sizeof(float));
            }
            else {
                memcpy(input_buffer, &inputFiles[i].samples[0][s * participant_num_input_frames[i]], participant_num_input_frames[i] * sizeof(float));
                memcpy(input_buffer + participant_num_input_frames[i], &inputFiles[i].samples[1][s * participant_num_input_frames[i]], participant_num_input_frames[i] * sizeof(float));
            }
            error_code = imm_input_audio_float(imm_instance, room_id, i, input_buffer, participant_num_input_frames[i]);
            if (error_code != IMM_ERROR_NONE) {
                /* Error */
                std::cout << "imm_input_audio_float for participant failed with error code " << error_code <<std::endl;
            }
        }

        /* Get the output audio for each participant */
        for (int i = 0; i < number_participants; i++) {
            error_code = imm_output_audio_float(imm_instance, room_id, i, output_buffer);
            if (error_code != IMM_ERROR_NONE) {
                /* Error */
                std::cout << "imm_output_audio_float for participant failed with error code " << error_code <<std::endl;
            }
            memcpy(&outputFiles[i].samples[0][s * OUTPUT_NUM_FRAMES], output_buffer, OUTPUT_NUM_FRAMES * sizeof(float));
            memcpy(&outputFiles[i].samples[1][s * OUTPUT_NUM_FRAMES], output_buffer + OUTPUT_NUM_FRAMES, OUTPUT_NUM_FRAMES * sizeof(float));
        }
        s = s+1;
    }

    /* Write the output files */
    for (int i = 0; i < number_participants; i++) {
        std::string file_name = "outfile_" + std::to_string(i+1) + ".wav";
        outputFiles[i].setBitDepth(16);
        outputFiles[i].save(file_name, AudioFileFormat::Wave);
    }

    /* Remove participants */
    for (int i = 0; i < number_participants; i++) {
        error_code = imm_remove_participant(imm_instance, room_id, i);
        if (error_code != IMM_ERROR_NONE) {
            /* Error */
            std::cout << "imm_remove_participant failed with error code " << error_code <<std::endl;
        }
    }

    /* Destroy room */
    error_code = imm_destroy_room(imm_instance, room_id);
    if (error_code != IMM_ERROR_NONE) {
        /* Error */
        std::cout << "imm_destroy_room failed with error code " << error_code <<std::endl;
    }

    /* Destroy library */
    error_code = imm_destroy_library(imm_instance);
    if (error_code != IMM_ERROR_NONE) {
        /* Error */
        std::cout << "imm_destroy_library failed with error code " << error_code <<std::endl;
    }

	return 0;
}
