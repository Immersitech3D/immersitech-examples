// Standard C includes
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Make sure to include the Immersitech Header file
#include "immersitech.h"

// Define these constants for your system specifications
#define OUTPUT_SAMPLING_RATE 48000							// Immersitech currently supports only 8 kHz, 16 kHz, 24 kHz, 32 kHz, or 48 kHz output
#define OUTPUT_NUM_FRAMES 960								// Currently only supporting 480, 512, 960, or 1024
#define OUTPUT_NUM_CHANNELS 2								// THIS MUST BE STEREO (2) TO HEAR 3D AUDIO. Mono output will not allow for 3D listening experiences.
#define INTERLEAVED true									// If using stereo this indicates if your input and output are interleaved or deinterleaved
#define SPATIAL_QUALITY 5									// Possible values of 1 through 5. Higher value for higher audio quality, lower values for better performance

// WAV header for reading and writing files
typedef struct header
{
	unsigned char chunk_id[4]; // 4 bytes
	unsigned int chunk_size; // 4 bytes
	unsigned char format[4]; // 4 bytes
	unsigned char subchunk1_id[4]; // 4 bytes
	unsigned int subchunk1_size; // 4 bytes
	unsigned short audio_format; // 2 bytes
	unsigned short num_channels; // 2 bytes
	unsigned int sample_rate; // 4 bytes
	unsigned int byte_rate; // 4 bytes
	unsigned short block_align; // 2 bytes
	unsigned short bits_per_sample; // 2 bytes
	unsigned char subchunk2_id[4]; //4 bytes
	unsigned int subchunk2_size; // 4 bytes
} header;

typedef struct header* header_p;
// WAV header for reading and writing files

int main(int argc, char** argv)
{

	// Check the command line parameters
	if (argc < 2) {
		printf("\nUsage: immersitech_example -input_file_1 -input_file_2 [-input_file_N..]");
		printf("\nUsage: You may input any number of input files to all be rendered in the same 3D scene");
		printf("\nUsage: Make sure your input files are signed 16 bit integer WAV files\n\n");
		return 0;
	}

	// We will keep track of how many participants there are as the number of files input on the command line
	int number_participants = argc - 1;

	// We can save information about each participant to reference them later
	int i;
	int* participant_sampling_rates		= (int*)malloc(number_participants * sizeof(int));	// The sampling rate of each input file
	int* participant_num_channels		= (int*)malloc(number_participants * sizeof(int));	// The number of channels in each input file
	int* participant_num_input_frames	= (int*)malloc(number_participants * sizeof(int));	// How many frames this participant will need to input each buffer

	// Initialize buffers to read input files and write to output files
	short *input_buffer  = (short*)malloc( OUTPUT_NUM_FRAMES * OUTPUT_NUM_CHANNELS * sizeof(short));
	short *output_buffer = (short*)malloc( OUTPUT_NUM_FRAMES * OUTPUT_NUM_CHANNELS * sizeof(short));

	// Open input and output files
	// Input files will simulate input audio from each individual participant
	// Output files let you review what each participant hears
	FILE** input_files  = (FILE**)malloc(number_participants * sizeof(FILE*) );
	FILE** output_files = (FILE**)malloc(number_participants * sizeof(FILE*) );
	char output_file_name[100];
	for(i = 0; i < number_participants; i++) {
    	snprintf(output_file_name, sizeof(char) * 100, "./output_file_%i.wav", i+1);
		input_files[i]  = fopen(argv[i+1],  "rb");
		output_files[i] = fopen(output_file_name, "wb");
	}

	// Read in all the metadata about the input files
	header_p meta = (header_p)malloc(sizeof(header));
	for(i = 0; i < number_participants; i++) {
		fread( meta, 1, sizeof(header), input_files[i]);
		participant_sampling_rates[i] 	= meta->sample_rate;
		participant_num_channels[i]   	= meta->num_channels;
		participant_num_input_frames[i] = (OUTPUT_NUM_FRAMES * participant_sampling_rates[i]) / OUTPUT_SAMPLING_RATE;
	}

	// Write out metadata for the output files
	if (participant_num_channels[number_participants -1] == 1) {
		meta->subchunk2_size *= 2;
		meta->chunk_size = 36 + meta->subchunk2_size;
	}
	meta->num_channels = OUTPUT_NUM_CHANNELS;
	meta->sample_rate  = OUTPUT_SAMPLING_RATE;
	for(i = 0; i < number_participants; i++) {
		fwrite( meta, 1, sizeof(header), output_files[i] );
	}

	// Here we define the path to your license file (Change the path to match where your actual license is located relative to the executable)
	#ifdef _MSC_VER
		char license_path[] = "Immersitech_Engineering_sound_manager_license_key.dat";
	#else
		char license_path[] = "./Immersitech_Engineering_sound_manager_license_key.dat";
	#endif

	// In this example we will turn on logging to help with debugging.
	// In a production product we could leave logging turned off as it is by default
	imm_enable_logging(true);

	// Initialize the Immersitech Library
	// This step must be done before we can take any other actions and is not thread safe
	imm_error_code error_code;
	imm_library_configuration output_config = { OUTPUT_SAMPLING_RATE, OUTPUT_NUM_FRAMES, OUTPUT_NUM_CHANNELS, INTERLEAVED, SPATIAL_QUALITY };
	imm_handle imm = imm_initialize_library(license_path, NULL, NULL, output_config, &error_code);

	// Now we can check the version of our library and information about our license key
	printf("\nThe Immersitech Libraries are %s", imm_get_version());
	printf("\nLicense Key Information: %s\n", imm_get_license_info(imm));

	// Now that everything is set up, first we create a room to add participants to
	int room_id = 0;
	imm_create_room(imm, room_id);

	// Next we can go and add participants to our room
	// As you can see here, each participant can have a different input configuration
	imm_participant_configuration input_config;
	input_config.type = IMM_PARTICIPANT_REGULAR;
	for(i = 0; i < number_participants; i++) {
		input_config.input_sampling_rate = participant_sampling_rates[i];
		input_config.input_number_channels = participant_num_channels[i];
		imm_add_participant(imm, room_id, i, "participant", input_config);
	}

	// Move the Participant's to their location in the room
	// The unit for the x,y,z coordinates is Centimeters
	imm_position position = { 0,0,0 };
	imm_heading heading = { 0,0 };
	for (i = 0; i < number_participants; i++) {
		// Randomize the positions of the participants. Here you can change this code to place them in your desired (x,y,z) positions
		// Also be aware of the heading for each participant. The direction they are facing will change their perspective of the 3d scene.
		if (i % 2) {
			position.x = i * 20;
			position.z = i * 30;
		}
		else {
			position.x = i * -50;
			position.z = i * 50;
			heading.azimuth_heading = 135;
		}
		imm_set_participant_position(imm, room_id, i, position, heading);
	}
	

	// Set Participant's audio settings to which effects they want to hear
	for(i = 0; i < number_participants; i++) {
		imm_set_participant_state(imm, room_id, i, IMM_CONTROL_ANC_ENABLE, 			1);
		imm_set_participant_state(imm, room_id, i, IMM_CONTROL_AGC_ENABLE, 			1);
		imm_set_participant_state(imm, room_id, i, IMM_CONTROL_MIXING_3D_ENABLE, 	1);
		imm_set_participant_state(imm, room_id, i, IMM_CONTROL_DEVICE, 				IMM_DEVICE_HEADPHONE);
	}

	// You can also adjust the settings for all the participants in a room at once instead of looping through a list of participants
	imm_set_all_participants_state(imm, room_id, IMM_CONTROL_MIXING_3D_ATTENUATION, 6);
	imm_set_all_participants_state(imm, room_id, IMM_CONTROL_MIXING_3D_REVERB_ENABLE, 1);
	imm_set_all_participants_state(imm, room_id, IMM_CONTROL_MIXING_3D_MAX_DISTANCE, 300);

	// Now we are ready to process the audio
	// In this example, loop through a file and buffer it as you would see in real-time application
	while ( !feof(input_files[0]) )
	{
		// Read in one buffer of audio from each file
		// Input each buffer into its respective Immersitech Participant within the imm, room_id,
		for(i = 0; i < number_participants; i++) {
			fread(input_buffer, 1, participant_num_input_frames[i] * participant_num_channels[i] * sizeof(short), input_files[i]);
			imm_input_audio_short(imm, room_id, i, input_buffer, participant_num_input_frames[i]);
		}

		// Now that all the audio is entered into the room, for this cycle, we are ready to generate the outputs
		// Generate the output for each participant and save it to their output file
		for(i = 0; i < number_participants; i++) {
			imm_output_audio_short(imm, room_id, i, output_buffer);
			fwrite(output_buffer, 1, OUTPUT_NUM_FRAMES * OUTPUT_NUM_CHANNELS * sizeof(short), output_files[i]);
		}
	}

	// Clean Up Process:
	// Remove all participants from the room
	for(i = 0; i < number_participants; i++) {
		imm_remove_participant( imm, room_id, i );
	}

	// Destroy the room
	imm_destroy_room( imm, room_id);

	// Destroy the library
	// This must be the last step in your program and is not thread safe
	imm_destroy_library(imm);

	// Close input and output files and free input / output buffers
	for(i = 0; i < number_participants; i++) {
		fclose(input_files[i] );
		fclose(output_files[i]);
	}
	free(input_files);
	free(output_files);
	free(input_buffer);
	free(output_buffer);
	free(meta);
	free(participant_sampling_rates);
	free(participant_num_channels);
	free(participant_num_input_frames);
}
