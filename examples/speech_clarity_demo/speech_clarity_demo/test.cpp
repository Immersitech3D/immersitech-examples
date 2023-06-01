#include "immersitech.h"
#include "immersitech_logger.h"

#include "audiofile.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define SAMPLERATE_HZ (48000)
#define BLOCKSIZE_SAMPLES (480)

imm_library_configuration m_configuration;
imm_handle m_imm_instance;
int m_room_id = 0;
imm_participant_configuration source_config;
imm_participant_configuration listener_config;
imm_error_code error_code;

/*

This command line tool takes an input wav file and processes it through the SDK. The processed
audio is stored as a wav file.

IMPORTANT:
The input wav file MUST be mono and have a sample rate of 48kHz.

SYNTAX:
immersitech_sdk_test.exe <input.wav> <output.wav> <ns_enable> <aeq_enable> <agc_enable>

*/
int main(int argc, const char* argv[])
{
    if (argc < 6)
    {
        std::cout << "Usage: \nimmersitech_sdk_test infile.wav outfile.wav enable_noise_suppression enable_auto_eq enable_gain_control \nThe input wav file MUST be mono and have a sample rate of 48kHz. \nex: immersitech_sdk_test 0338noisySpeech0dB.wav out.wav 1 1 1" << std::endl;
        return 1;
    }
    
	const char* input_audio_file = argv[1];
    const char* outputaudio = argv[2];
    int ns_enable = atoi(argv[3]);
	int aeq_enable = atoi(argv[4]);
	int agc_enable = atoi(argv[5]);
    
    imm_enable_logging(true);
    imm_set_log_level(imm_log_level::IMM_LOG_DEBUG);

    /* Initialize IMM library */
    m_configuration.interleaved = false;
    m_configuration.output_number_channels = 1;
    m_configuration.output_number_frames = BLOCKSIZE_SAMPLES;
    m_configuration.output_sampling_rate = SAMPLERATE_HZ;
    m_configuration.spatial_quality = 3;

    m_imm_instance = imm_initialize_library("../../../license/Immersitech_Engineering_sound_manager_license_key.dat", NULL, NULL, m_configuration, &error_code);
    if (error_code != IMM_ERROR_NONE)
    {
        /* Error */
        std::cout << "imm_initialize_library failed with error code " << error_code <<std::endl;
    }
    
    /* Create room */
    error_code = imm_create_room(m_imm_instance, m_room_id);
    if (error_code != IMM_ERROR_NONE) {
        /* Error */
        std::cout << "imm_create_room failed with error code " << error_code <<std::endl;
    }

    /* Since processing is going "one-way" in this demo, we make one participant a Source, and one a Listener */
    source_config.input_number_channels = 1;
    source_config.input_sampling_rate = SAMPLERATE_HZ;
    source_config.type = IMM_PARTICIPANT_SOURCE_ONLY;
    
    listener_config.input_number_channels = 1;
    listener_config.input_sampling_rate = SAMPLERATE_HZ;
    listener_config.type = IMM_PARTICIPANT_LISTENER_ONLY;

    /* Add participants to room */
    std::string participant0 = "Speaker";
    error_code = imm_add_participant(m_imm_instance, m_room_id, 0, participant0.c_str(), source_config);
    if (error_code != IMM_ERROR_NONE) {
        /* Error */
        std::cout << "imm_add_participant failed with error code " << error_code <<std::endl;
    }

    /* Add participants to room */
    std::string participant1 = "Listener";
    error_code = imm_add_participant(m_imm_instance, m_room_id, 1, participant1.c_str(), listener_config);
    if (error_code != IMM_ERROR_NONE) {
        /* Error */
        std::cout << "imm_add_participant failed with error code " << error_code <<std::endl;
    }

    /*************************/
    /* Set participant state */
    /*************************/
    error_code = imm_set_all_participants_state(m_imm_instance, m_room_id, IMM_CONTROL_ANC_ENABLE, ns_enable);
    if (error_code != IMM_ERROR_NONE) {
        /* Error */
        std::cout << "imm_set_all_participants_state failed with error code " << error_code <<std::endl;
    }

    error_code = imm_set_all_participants_state(m_imm_instance, m_room_id, IMM_CONTROL_AGC_ENABLE, agc_enable);
    if (error_code != IMM_ERROR_NONE) {
        /* Error */
        std::cout << "imm_set_all_participants_state failed with error code " << error_code <<std::endl;
    }

    error_code = imm_set_all_participants_state(m_imm_instance, m_room_id, IMM_CONTROL_AUTO_EQ_ENABLE, aeq_enable);
    if (error_code != IMM_ERROR_NONE) {
        /* Error */
        std::cout << "imm_set_all_participants_state failed with error code " << error_code <<std::endl;
    }

    error_code = imm_set_all_participants_state(m_imm_instance, m_room_id, IMM_CONTROL_MIXING_3D_ENABLE, 0);
    if (error_code != IMM_ERROR_NONE) {
        /* Error */
        std::cout << "imm_set_all_participants_state failed with error code " << error_code <<std::endl;
    }

    error_code = imm_set_all_participants_state(m_imm_instance, m_room_id, IMM_CONTROL_MIXING_3D_REVERB_ENABLE, 0);
    if (error_code != IMM_ERROR_NONE) {
        /* Error */
        std::cout << "imm_set_all_participants_state failed with error code " << error_code <<std::endl;
    }

    AudioFile<float> inputAudio;
    bool loadedOK = inputAudio.load(input_audio_file);
    if (loadedOK == false) {
        /* Error */
        std::cout << "Failed to load input file " << input_audio_file << std::endl;
    }

    int numberOfSamples = inputAudio.getNumSamplesPerChannel();
    int samplesRead = 0;
    int samplesWritten = 0;
    int maxSamples = 0;

    AudioFile<float> outputfile;
    outputfile.setSampleRate(SAMPLERATE_HZ);
    outputfile.setNumChannels(1);
    outputfile.setNumSamplesPerChannel(numberOfSamples + SAMPLERATE_HZ);
    maxSamples = outputfile.getNumSamplesPerChannel();

    while (1)
    {
        float sampleBlock[480] = { 0.0f };
        float tmp[480] = { 0.0f };
        float sampleBlockOut[480] = { 0.0f };

        for (int i = 0; i < BLOCKSIZE_SAMPLES; i++)
        {
            if (samplesRead < numberOfSamples)
            {
                sampleBlock[i] = inputAudio.samples[0][samplesRead];
                samplesRead++;
            }
        }
        
        /* Participant 0 */
        error_code = imm_input_audio_float(m_imm_instance, 0, 0, sampleBlock, (int)BLOCKSIZE_SAMPLES);
        if (error_code != IMM_ERROR_NONE)
        {
            /* Error */
            std::cout << "imm_input_audio_short for participant 0 failed with error code " << error_code <<std::endl;
        }
        
        /* Participant 1 */
        /* If you had multiple audio sources, you can add their audio to the room in the same way */
        /*
        error_code = imm_input_audio_float(m_imm_instance, 0, 1, sampleBlock, (int)BLOCKSIZE_SAMPLES);
        if (error_code != IMM_ERROR_NONE)
        {
            std::cout << "imm_input_audio_short for participant 1 failed with error code " << error_code <<std::endl;
        }
        */

        /* Get output audio for participant 1 */
        error_code = imm_output_audio_float(m_imm_instance, 0, 1, sampleBlockOut);
        if ((error_code != IMM_ERROR_NONE) && (error_code != IMM_ERROR_NO_INPUT_AUDIO))
        {
            /* Error */
            std::cout << "imm_output_audio_short for participant 1 failed with error code " << error_code <<std::endl;
        }
        
        /* Get output audio for participant 0 */
        /* If Participant 1 was also inputting audio, Participant 0 could access it in the same way*/
        /*
        error_code = imm_output_audio_float(m_imm_instance, 0, 0, sampleBlockOut);
        if ((error_code != IMM_ERROR_NONE) && (error_code != IMM_ERROR_NO_INPUT_AUDIO))
        {
            std::cout << "imm_output_audio_short for participant 0 failed with error code " << error_code <<std::endl;
        }
        */

        for (int i = 0; i < BLOCKSIZE_SAMPLES; i++)
        {
            if (samplesWritten < maxSamples)
            {
                outputfile.samples[0][samplesWritten] = sampleBlockOut[i];
                samplesWritten++;
            }
        }

        if (samplesRead >= numberOfSamples)
        {
            /* Done */
            outputfile.setBitDepth(16);
            outputfile.setNumSamplesPerChannel(samplesWritten);
            outputfile.save(outputaudio, AudioFileFormat::Wave);
            break;
        }
    }

    /* Remove participants */
    error_code = imm_remove_participant(m_imm_instance, m_room_id, 0);
    if (error_code != IMM_ERROR_NONE) {
        /* Error */
        std::cout << "imm_remove_participant failed with error code " << error_code <<std::endl;
    }

    error_code = imm_remove_participant(m_imm_instance, m_room_id, 1);
    if (error_code != IMM_ERROR_NONE) {
        /* Error */
        std::cout << "imm_remove_participant failed with error code " << error_code <<std::endl;
    }

    /* Destroy room */
    error_code = imm_destroy_room(m_imm_instance, m_room_id);
    if (error_code != IMM_ERROR_NONE) {
        /* Error */
        std::cout << "imm_destroy_room failed with error code " << error_code <<std::endl;
    }

	return 0;
}
