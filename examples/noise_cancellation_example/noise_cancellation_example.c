// Standard C includes
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Make sure to include the Immersitech Header file
#include "immersitech.h"

// Define these constants for your system specifications
#define OUTPUT_SAMPLING_RATE 48000			// Immersitech currently supports only 8 kHz, 16 kHz, 24 kHz, 32 kHz, or 48 kHz output
#define OUTPUT_NUM_FRAMES 480				// 480 Frames at 48 kHz is 10 milliseconds of data
#define OUTPUT_NUM_CHANNELS 1				// In this case we want mono output because we are just doing noise cancellation on a mono input
#define INTERLEAVED true					// This doesn't matter for mono, but if using stereo this would indicate if your input and output is interleaved or deinterleaved
#define SPATIAL_QUALITY 1					// This value defines the accuracy of 3D rendering. In this example there is no 3D, so set it to the lowest value to save RAM.

// WAV header for reading and writing files
typedef struct wav_header {
	unsigned char chunk_id[4];
	unsigned int chunk_size;
	unsigned char format[4];
	unsigned char subchunk1_id[4];
	unsigned int subchunk1_size;
	unsigned short audio_format;
	unsigned short num_channels;
	unsigned int sample_rate;
	unsigned int byte_rate;
	unsigned short block_align;
	unsigned short bits_per_sample;
	unsigned char subchunk2_id[4];
	unsigned int subchunk2_size;
} wav_header;
// WAV header for reading and writing files

int main(int argc, char **argv)
{
	// Check the command line parameters
	if (argc < 3){
		printf("\nUsage: noise_cancellation_example -input_file_1 -output_file_1");
		printf("\nUsage: Make sure your input file is a signed 16 bit WAV file\n\n");
		return 0;
	}
	
	// Open input and output files
	// Input files will simulate input audio from each individual participant
	FILE* input_file = fopen(argv[1], "rb");
	if (input_file == NULL) {
		printf("\nCould not open %s", argv[1]);
		return 0;
	}

	// Output files let you review what each participant hears
	FILE* output_file = fopen(argv[2], "wb");
	if (output_file == NULL) {
		printf("\nCould not open %s", argv[2]);
		return 0;
	}

	// Read in data about input file
	struct wav_header wav_meta_data;
	fread(&wav_meta_data, 1, sizeof(wav_header), input_file);
	int input_rate = wav_meta_data.sample_rate;
	int input_channels = wav_meta_data.num_channels;
	printf("Input Configuration: %s || %i Hz || %i Channel(s)\n\n", argv[1], input_rate, input_channels);

	// Write WAV Header data to the output file
	if (wav_meta_data.sample_rate != OUTPUT_SAMPLING_RATE) {
		wav_meta_data.subchunk2_size *= (OUTPUT_SAMPLING_RATE / wav_meta_data.sample_rate);
		wav_meta_data.sample_rate = OUTPUT_SAMPLING_RATE;
		wav_meta_data.chunk_size = 36 + wav_meta_data.subchunk2_size;
	}
	fwrite(&wav_meta_data, 1, sizeof(wav_header), output_file);

	// Calculate the number of frames we must receive from each input
	int input_num_frames = (OUTPUT_NUM_FRAMES * input_rate) / OUTPUT_SAMPLING_RATE;

	// Initialize buffers to read input files and write to output files
	short* input_buffer = (short*)malloc(input_num_frames * input_channels * sizeof(short));
	short* output_buffer = (short*)malloc(OUTPUT_NUM_FRAMES * OUTPUT_NUM_CHANNELS * sizeof(short));

	// Here define the path to the license file on your computer
#ifdef _MSC_VER
	char license_path[] = "Immersitech_Engineering_sound_manager_license_key.dat";
#else
	char license_path[] = "./Immersitech_Engineering_sound_manager_license_key.dat";
#endif

	// For this example we will turn logging on to help with any debugging needs
	// For production use, we can leave logging turned off
	imm_enable_logging(true);

	// Initialize the Immersitech Library
	// This must be done before anything else will work
	imm_error_code error_code;
	imm_library_configuration output_config = { OUTPUT_SAMPLING_RATE, OUTPUT_NUM_FRAMES, OUTPUT_NUM_CHANNELS, INTERLEAVED, SPATIAL_QUALITY };
	imm_handle imm = imm_initialize_library( license_path, NULL, NULL, output_config, &error_code);

	// Here we can check the version of the library that we are using
	printf("\nUsing Immersitech version: %s", imm_get_version());

	// We can also check our license key is valid as well
	printf("\nLicense Key Info: %s", imm_get_license_info(imm));

	// Create and initialize a room to put participants into
	int room_id = 0;
	imm_create_room(imm, room_id);

	// Add Participants into our room
	int ID_1 = 1;
	int ID_2 = 2;
	imm_participant_configuration input_config = { input_rate, input_channels, IMM_PARTICIPANT_REGULAR };
	imm_add_participant(imm, room_id, ID_1, "participant_1", input_config);
	imm_add_participant(imm, room_id, ID_2, "participant_2", input_config);

	// Turn on the noise cancellation for all the participants
	imm_set_all_participants_state(imm, room_id, IMM_CONTROL_ANC_ENABLE, 1);

	// Now we are ready to process the audio
	// In this example, loop through a file and buffer it as you would see in real-time application
	while ( !feof(input_file) )
	{
		// Read in one buffer of audio from each file
		// Input each buffer into its respective Immersitech Participant within the room
		fread(input_buffer, 1, input_num_frames * input_channels * sizeof(short), input_file);
		imm_input_audio_short(imm, room_id, ID_1, input_buffer, input_num_frames);
		
		// Now that all the audio is entered into the room for this cycle, we are ready to generate the outputs
		// Generate the output for each participant and save it to their output file
		imm_output_audio_short(imm, room_id, ID_2, output_buffer);
		fwrite(output_buffer, 1, OUTPUT_NUM_FRAMES * OUTPUT_NUM_CHANNELS * sizeof(short), output_file);
	}

	// Clean Up Process:
	// Remove all participants from the room
	imm_remove_participant( imm, room_id, ID_1 );
	imm_remove_participant( imm, room_id, ID_2 );

	// Destroy the room
	imm_destroy_room(imm, room_id);

	// Close and free the library
	// This must be the last step in your program
	imm_destroy_library(imm);

	// Close input and output files and free input / output buffers
	fclose(input_file);
	fclose(output_file);
	free(input_buffer);
	free(output_buffer);
}
