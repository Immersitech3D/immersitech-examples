#pragma once

/// ******************************************************************************
/// Copyright (C) Immersitech 2021
/// The use of this code is bound by the Immersitech Terms of Use at:
/// https://immersitech.io/terms-of-use/
/// *****************************************************************************
/// *****************************************************************************
/// DESCRIPTION 
/// Immersitech Library Interface definition file
///
/// NOTES  
/// This is the Immersitech Library API
/// To learn more please send an email to info@immersitech.io
/// or visit https://www.immersitech.io/
///
/// Author: Isaac Mosebrook
/// Date Created: August 14, 2019
/// Date Modified: July 7, 2021
/// *****************************************************************************

#ifndef IMMERSITECH_LIBRARY_H_
#define IMMERSITECH_LIBRARY_H_

#include <stdbool.h>

#if HAVE_VISIBILITY
#define IMMERSITECH_API __attribute__((__visibility__("default")))
#elif defined _WIN32
#define IMMERSITECH_API __declspec(dllexport)
#else
#define IMMERSITECH_API
#endif

/// @brief A structure to describe a position within a three-dimensional space.
///
/// Note that the coordinate system uses centimeters.
/// Check the documentation section on the coordinate system to see the alignment of the axes.
///
/// @param x The position of a participant along the x-axis.
/// @param y The position of a participant along the y-axis.
/// @param z The position of a participant along the z-axis.
typedef struct imm_position {
	int x;
	int y;
	int z;
} imm_position;

/// @brief A structure to describe the direction a participant is facing in three-dimensional space.
///
/// The azimuth angle is with respect to the x and z plane.
/// The elevation angle is with respect to the y and z plane.
///
/// @param azimuth_heading The azimuth angle the participant is facing. Allowable azimuth headings are from -180 to 180.
/// @param elevation_heading The elevation angle the participant is facing. Allowable elevation headings are from -90 to 90.
typedef struct imm_heading {
	int azimuth_heading;
	int elevation_heading;
} imm_heading;

/// @brief A structure to describe a seat within a room.
///
/// A participant in a room can occupy one seat which is located in a position. 
/// Its heading describes the direction it will automatically turn to face the center point of the room.
///
/// @param id A unique identifier for this seat.
/// @param position The position of the seat in three-dimensional space.
/// @param heading The direction a participant will face when they are placed in this seat.
typedef struct imm_seat {
	int id;
	imm_position position;
	imm_heading heading;
} imm_seat;

/// @brief Audio Controls you can change in real-time
///
/// A list of all the audio effects available in the Immersitech Library.
/// For any control that ends with ENABLE, the only two possible values are 1 or 0 (enabled or disabled respectively).
typedef enum imm_audio_control {
	IMM_CONTROL_START,						///< This value allows you to identify the start of the enum
	IMM_CONTROL_STEREO_BYPASS_ENABLE,		///< If the stereo bypass is enabled, all effects will be bypassed, meaning no processing will occur. Audio will pass unchanged. Deafults to 0.
	IMM_CONTROL_MUTE_ENABLE,				///< If a participant's mute is enabled, their input audio will not be passed to the other participants. Defaults to 0.
	IMM_CONTROL_ANC_ENABLE,					///< If a participant has ANC enabled, all noise in their input audio will be automatically cancelled. Defatuls to 0.
	IMM_CONTROL_AGC_ENABLE,					///< If a participant has AGC enabled, their input audio will be automatically adjusted to match the volume of the other participants. Defaults to 0.
	IMM_CONTROL_AUTO_EQ_ENABLE,				///< If a participant has Auto EQ enabled, their input audio will automatically adjusted to maximize the clarity of their speech. Defaults to 0.
	IMM_CONTROL_MIXING_3D_ENABLE,			///< If a participant has 3D mixing enabled, they will hear all other participants spatially rendering in 3D space. Defaults to 0.
	IMM_CONTROL_MIXING_3D_ATTENUATION,		///< If 3D mixing is enabled, this control will determine how the volume of a source drops off as they move away from a listener. This parameter is given in dB drop per meter. A value of 0 means the volume will stay constant and not drop with distance. The default value is 6 dB. Allowed values are 0 to 40 dB.
	IMM_CONTROL_MIXING_3D_MAX_DISTANCE,		///< If 3D mixing is enabled, this control will determine the max distance up to which attenuation will apply. If a source is at a distance greater than this value, it volume will no longer be attenutated further. Default value is 300. Any non-negative value is allowable.
	IMM_CONTROL_MIXING_3D_REVERB_ENABLE,	///< If 3D mixing is enabled, this control will determine if reverb is applied to the 3D rendering. It is enabled by default.
	IMM_CONTROL_DEVICE,						///< The device setting will help optimize 3D processing for a particular participant's listening device. Defaults to IMM_DEVICE_HEADPHONE.
	IMM_CONTROL_HALF_SPAN_ANGLE,			///< If the participant is using speakers, they should set the Half Span Angle from 1 to 90 to match the positioning of their physical speakers. Defaults to 15.
	IMM_CONTROL_MASTER_GAIN,				///< The master gain setting will adjust the volume for a participant's output audio. It is given as a percent of nominal volume, from 0 to 100. Defaults to 100.
	IMM_CONTROL_WHISPER_ROOM,				///< If a participant's whisper room is set to a non-zero value from 1-100, they will be rendered inside the specified whisper room. Defaults to 0.
	IMM_CONTROL_SIDEBAR_ROOM,				///< If a participant's sidebar room is set to a non-zero value from 1-100, they will be rendered inside the specified sidebar room. Defatuls to 0.
	IMM_CONTROL_END							///< This value allows you to identify the end of the enum
}imm_audio_control;

/// @brief Output Device Types you may be listening on.
///
/// The type of device a participant is listening to the output on.
/// Note that speaker refers to a stereo loudspeaker pair.
/// If a participant isn't sure what type of device they have, it is always advised to default to IMM_DEVICE_HEADPHONE.
typedef enum imm_device_type {
	IMM_DEVICE_START,						///< This value allows you to identify the start of the enum
	IMM_DEVICE_HEADPHONE,					///< The listening device is a headphone, including in-ear, on-ear, and over-ear variants
	IMM_DEVICE_SPEAKER,						///< The listening device is a stereo loudspeaker pair. If this is selected, the participant should also set their IMM_CONTROL_HALF_SPAN_ANGLE.
	IMM_DEVICE_END							///< This value allows you to identify the end of the enum
}imm_device_type;

/// @brief Types of participants you can instantiate.
///
/// The rules will be enforced stricly. 
/// For example, if you create a source only participant and try to output audio, a error code will be returned and no processing will occur.
/// One possible benefit of source only participants would be sound effect audio or audio from a video being shown in a conference.
/// One possible benefit of listener only participants would be recording a conference.
typedef enum imm_participant_type {
	IMM_PARTICIPANT_START,					///< This value allows you to identify the start of the enum
	IMM_PARTICIPANT_REGULAR,				///< A regular participant will both input audio and receive other participant's audio
	IMM_PARTICIPANT_SOURCE_ONLY,			///< A source only participant will only input audio and will NOT receive other participant's audio
	IMM_PARTICIPANT_LISTENER_ONLY,			///< A listener only participant will only receive other participant's audio and will NOT input audio
	IMM_PARTICIPANT_END						///< This value allows you to identify the end of the enum
}imm_participant_type;

/// @brief All error codes that may be produced by the library
///
typedef enum imm_error_code {
	IMM_ERROR_START = -10001,				///< This value allows you to identify the start of the enum
	IMM_ERROR_NONE,							///< This error code indicates the function worked successfully without error
	IMM_ERROR_HANDLE_NULL,					///< This error code indicates you have not initialized your imm_handle properly yet.
	IMM_ERROR_DATA_NULL,					///< This error code indicates you tried to use a buffer or variable that wasn't allocated properly.
	IMM_ERROR_DATA_LENGTH,					///< This error code indicates there is a mismatch between the size of the data buffer you provided and the size required.
	IMM_ERROR_NUM_CHANNELS,					///< This error code indicates that you tried to specify a number of channels outside the allowed bounds. Currently only 1 or 2 channels are allowed.
	IMM_ERROR_SAMPLE_RATE,					///< This error code indicates that you tried to specify an output sampling rate that is unsupported or an input sampling rate that doesn't work with the output sampling rate.
	IMM_ERROR_SPATIAL_QUALITY,				///< This error code indicates that you specified an invalid spatial quality value. Allowable values are from 1 to 5.
	IMM_ERROR_INVALID_ROOM_ID,				///< This error code indicates that you requested a room with an ID that doesn't exist or tried to create a room with an ID that already existed.
	IMM_ERROR_INVALID_PARTICIPANT_ID,		///< This error code indicates that you requested a participant with an ID that doesn't exist or tried to add a participant with an ID that already existed.
	IMM_ERROR_INVALID_CONTROL,				///< This error code indicates the control specified is not a valid one with the bounds of imm_audio_control
	IMM_ERROR_INVALID_VALUE,				///< This error code indicates the value provided is outside the bounds specified for the various imm_audio_control. It can also indicate you provided an invalid function parameter in general.
	IMM_ERROR_INVALID_HEADING,				///< This error code indicates you provide a heading that was outside the allowed values. Please reference imm_heading for allowed values.
	IMM_ERROR_NO_INPUT_AUDIO,				///< This error code indicates that there was no input audio to be processed, meaning an output buffer could not be generated. Instead the output buffer will be zeroed.
	IMM_ERROR_PARTICIPANT_TYPE,				///< This error code matches the exceptions specified under imm_participant_type.
	IMM_ERROR_LIBRARY_NOT_YET_INITIALIZED,	///< This error code will be generated if you call any functions before initializing the library.
	IMM_ERROR_LICENSE_DATE_EXPIRED,			///< This error code indicates your license files no longer matches the date requirements. All audio effects will be bypassed.
	IMM_ERROR_LICENSE_VERSION_MISMATCH,		///< This error code indicates your license files version requirements no longer match. All audio effects will be bypassed.
	IMM_ERROR_LICENSE_TAMPERED,				///< This error code indicates your license file has been corrupted. All audio effects will be bypassed.
	IMM_ERROR_LICENSE_MISSING,				///< This error code indicates the path to your license file is invalid. All audio effects will be bypassed.
	IMM_ERROR_END							///< This value allows you to identify the end of the enum
}imm_error_code;

/// @brief A structure to describe the configuration of the Immersitech Library
///
/// @param output_sampling_rate The output sampling rate of the library. Currently the library only supports output sampling rates of 48000, 32000, 24000, 16000, 8000.
/// @param output_number_frames The number of frames generated in the output. Currently the library supports values of 480 (10 milliseconds), 512, 960 (20 milliseconds), and 1024.
/// @param output_num_channels The number of channels the library will output. Note if you are using any 3D effects, you must have at least 2 channels. Currently the library only supports 1 or 2 channel output.
/// @param interleaved If set to true, any multi-channel audio input or output from the library will be interleaved. If set to false, it will be de-intereleaved. For quick reference, a left right stereo signal that is interleaved would appear [ L R L R ] while it would appear [ L L R R] if deinterleaved.
/// @param spatial_quality This value describes the tradeof with 3D rendering between CPU speed and audio quality. A smaller value (1) gives better CPU / RAM performance but lesser 3D accuracy while a higher value (5) gives better audio quality with worse CPU / RAM performance. Acceptable integer values range from 1 to 5.
typedef struct imm_library_configuration {
	int output_sampling_rate;
	int output_number_frames;
	int output_number_channels;
	bool interleaved;
	int spatial_quality;
} imm_library_configuration;

/// @brief A structure to describe the configuration of a participant
///
/// Note that these settings will apply to the participant's input while the library configuration will determine their output sampling rate, channels, etc..
/// The library will resample an input with a different input sampling rate to match the library's output sampling rate.
/// @param input_sampling_rate The input sampling rate of the new participant.
/// @param input_num_channels The number of channels that this participant will input. Currently only supports 1 or 2 channels input.
/// @param type An imm_participant_type value that will describe whether this participant will input audio, output audio, or both.
typedef struct imm_participant_configuration {
	int input_sampling_rate;
	int input_number_channels;
	imm_participant_type type;
} imm_participant_configuration;

/// @brief Immersitech Library API Handler
///
/// This handle will help keep a pointer to the instance of the Immersitech Library you generate using the imm_initialize_library function.
/// You must have a valid imm_handle to use almost every function in the Immersitech API.
/// It therefore follows you should always call imm_initialize_library before any other function and check the error_code indicates it was a success before any other actions.
typedef void* imm_handle;

#ifdef __cplusplus
extern "C" {
#endif

	/// @brief Function to return the current version of the immersitech library
	///
	/// @return A string containing the version information for this instance of the Immersitech Library
	IMMERSITECH_API const char* imm_get_version();

	/// @brief Function to allocate memory for the immersitech library
	///
	/// This function only needs to be called once, but must be called before any other function is called.
	/// If you want to change any of these settings, you must first call imm_destroy_library, then call this function again.
	/// Neither this function nor imm_destroy_library are thread safe and so you should halt other operations while changing settings
	/// @param[in] license_file_name The path to your immersitech license file including the file name ("/path/my_license.dat").
	/// @param[in] room_layout_file_name The path to your room layout file including the file name ("/path/my_license.dat"). You may send NULL instead of a path if you do not wish to use any room layout features.
	/// @param[in] websocket_config_file_name The path to your websocket configuration file including the file name ("/path/my_license.dat"). You may send NULL instead of a path if you do not wish to use any websockets features.
	/// @param[in] configuration The settings for the initialization of the Immersitech library.
	/// @param[out] error_code An imm_error_code indicating if the call was a success or not.
	/// @return An imm_handle to your initialized library. This handle will be used for the other functions in the Immersitech API
	IMMERSITECH_API imm_handle imm_initialize_library(const char* license_file_name, const char* room_layout_file_name, const char* websocket_config_file_name, imm_library_configuration configuration, imm_error_code* error_code);

	/// @brief Function to frees all memory associated with the immersitech library
	///
	/// This function needs to be called only once at the very end of your program when everything else is finished.
	/// Once this function is called, you won't be able to call any other functions unless you reinitialize the library.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_destroy_library(imm_handle handle);

	/// @brief Function to return information about your current license key
	///
	/// Note that the returned string will be formatted as JSON so that you can easily parse information programmatically
	/// You must call imm_initialize_library successfully before this function will work.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @return A string formatted as JSON containing information about the license key you used to initialize the library
	IMMERSITECH_API const char* imm_get_license_info(imm_handle handle);

	/// @brief Function to get all the information about the way you configured the Immersitech Library
	///
	/// Note that this function will return invalid information (-1) until you initialize the library.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @return An imm_library_configuration filled with information about your setup
	IMMERSITECH_API imm_library_configuration imm_get_library_configuration(imm_handle handle);

	/// @brief Function to allocate memory and initialize a new room
	///
	/// Creates a new room that you can begin adding participants to.
	/// Note that this new room will be automatically set to your default room layout.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID you'd like to give your new room. Note that it must be a unique id and cannot be shared with another room.
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_create_room(imm_handle handle, int room_id);

	/// @brief Function to removes all remaining participants from a room and frees all the data allocated for a room
	///
	/// Once you have destroyed the room, you can reuse the room_id for a new room if you'd like.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID for the room you'd like to destroy.
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_destroy_room(imm_handle handle, int room_id);

	/// @brief Function to add a new pariticipant into a room
	///
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID for the room you'd like to add the new participant to.
	/// @param[in] participant_id The ID you'd like to give to the new participant. Note that it cannot be the same as the id of a participant already in this specific room.
	/// @param[in] participant_name A human readable string name for the participant. This will allow easier developer identification of each participant's purpose.
	/// @param[in] config The various configuration settings for the participant.
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_add_participant(imm_handle handle, int room_id, int participant_id, const char* participant_name, imm_participant_configuration config);

	/// @brief Function to remove a participant from a room
	///
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID for the room you'd like to remove the participant from.
	/// @param[in] participant_id The ID of the participant you'd like to remove.
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_remove_participant(imm_handle handle, int room_id, int participant_id);

	/// @brief Function to turn on or off logging within the Immersitech Library
	/// 
	/// If you disable logging by sending false to this function, nothing will ever be logged. No messages will be printed.
	/// If you enable logging by sending true to this function. The Immersitech Library will log all messages equal to or above the set logging level.
	/// By default, Immersitech Library will log all messages to the standard output.
	/// By default, Immersitech Library will set the log level to Warnings and above only.
	/// @param[in] enable This bool dictates whether you'd like to enable logging or not
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_enable_logging(bool enable);

	/// @brief Function to set the log level of the Immersitech logger
	/// 
	/// The logger will only log messages that are equal to or greater than the currently set log level.
	/// @param[in] level The log level you'd like to set. check the imm_log_level enum for details on which integer value corresponds to which log level
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_set_log_level(int level);

	/// @brief Function to view all the available room layouts as a JSON formatted string
	///
	/// The returned string will be a duplicate of the JSON in your room layout file you provided.
	/// If you did not provide a room layout file, the JSON will show the default room layout.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @return All the available room layouts  formatted as a JSON string
	IMMERSITECH_API const char* imm_get_json_all_room_layouts(imm_handle handle);

	/// @brief Function to view which room layout a paritcular room is currently set to
	///
	/// The integer value refers to the position of the layout in the room_list array in the room layout file.
	/// For example, if you have three layouts in your room layout file, a returned value of 1 would indicate this room's layout
	/// is set to the first room layout in the room_list array.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID of the room you would like to know the layout of
	/// @param[out] layout_id The ID of the room layout the room is currently set to
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_get_room_layout(imm_handle handle, int room_id, int* layout_id);

	/// @brief Function to set a room's layout
	///
	/// The integer value refers to the position of the layout in the room_list array in the room layout file.
	/// For example, if you have three layouts in your room layout file, a returned value of 1 would indicate this room's layout
	/// is set to the first room layout in the room_list array.
	/// When you switch layouts, the library will automatically move participants to the new seating positions consistent with the new layout.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID of the room you would like to set the layout of
	/// @param[in] layout_id The ID of the room layout you'd like to set the room to
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_set_room_layout(imm_handle handle, int room_id, int layout_id);

	/// @brief Function to link a custom event manager with the library
	/// 
	/// You can create a custom event manager by implementing the abstract IMM_EVENT_manager class.
	/// This custom class will define what code you'd like to be triggered when certain events happen.
	/// In order for the Immersitech Library to call your custom actions after an event, you must add it using this function.
	/// Note that you must manually initialize and free your own object, the immersitech library will NOT free the memory for you.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] event_manager A pointer to your initiated custom event manager object. 
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_add_event_manager(imm_handle handle, void* event_manager);

	/// @brief Function to enable the websocket server for controlling settings via websockets
	/// 
	/// Note: All websocket functionality is disabled on the client side version of the library
	/// If never call this function, the websocket server features will remain entirely disabled through your whole usage.
	/// Once you enable the websocket server through this function, clients may begin connecting and sending commands.
	/// See the client websocket API documentation for details on client commands
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] enable A bool to indicate if you would like to turn the websocket server on
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_enable_websockets(imm_handle handle, bool enable);

	/// @brief Function to broadcast any arbitrary message through the websocket server to all clients
	/// 
	/// This function will send any string you provide and broadcast it to all clients connected to the websocket server.
	/// If you are using the client side version of the library, this function will be disabled.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID of the room the custom message is related to
	/// @param[in] message A string containing the message you'd like to broadcase
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_send_custom_websocket_event(imm_handle handle, int room_id, const char* message);

	/// @brief Function to learn how many rooms are currently active
	/// 
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[out] room_count This parameters will store the number of active rooms
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_get_room_count(imm_handle handle, int* room_count);

	/// @brief Function to learn how many participants are currently in a given room
	/// 
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID of the room the participants are in
	/// @param[out] participant_count This variable will store the number of participants currently in the given room
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_get_participant_count(imm_handle handle, int room_id, int* participant_count);

	/// @brief Function view the name of a particular participant
	/// 
	/// You must initialize your value buffer to a reasonable length based on how long the names you allow are.
	/// If your buffer is not long enough to store the full name, an error will be returned.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID of the room the participant is in
	/// @param[in] participant_id The ID of the participant whose name is to be learned
	/// @param[out] value The name will be written to this value buffer that you provide
	/// @param[in] value_length The size of the buffer that you allocated value to
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_get_participant_name(imm_handle handle, int room_id, int participant_id, char* value, int value_length);

	/// @brief Function to set the name of a participant
	/// 
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID of the room the participant is in
	/// @param[in] participant_id The ID of the participant whose name you'd like to change
	/// @param[in] participant_name The new name for the participant
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_set_participant_name(imm_handle handle, int room_id, int participant_id, const char* participant_name);

	/// @brief Function to change an audio control for every participant in a room all at once
	/// 
	/// If you plan to change a settings for everyone in a room, you can use this as a convenience instead of looping through all the participants and calling imm_set_participant_state
	/// For example, maybe a host in your conference call sets noise cancellation active for all participants.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID of the room the participants are in
	/// @param[in] control_to_edit The imm_audio_control you'd like to change the setting of
	/// @param[in] value The new value of the given control
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_set_all_participants_state(imm_handle handle, int room_id, imm_audio_control control_to_edit, int value);

	/// @brief Function move a participant to a new seat
	/// 
	/// You can only move a participant to a seat defined in the current room's room layout.
	/// Note that seats are 1 indexed, meaning the first seat in the array is seat 1.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID of the room the participant is in
	/// @param[in] participant_id The ID of the participant that is moving seats
	/// @param[in] seat_id The ID of the new seat
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_set_participant_seat(imm_handle handle, int room_id, int participant_id, int seat_id);

	/// @brief Function get information about a participant's current seat
	/// 
	/// Note that if the seat's ID is -1, this means the participant is currently not in a seat.
	/// This is only possible in an open room layout where participants can move to an arbitrary x, y, z position.
	/// You must initialize the seat object before calling this function as the information will be written directly to it.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID of the room the participant is in
	/// @param[in] participant_id The ID of the participant
	/// @param[out] seat The seat the participant is currently in. View imm_seat to learn about all the properties of a seat object.
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_get_participant_seat(imm_handle handle, int room_id, int participant_id, imm_seat* seat);

	/// @brief Function to get information about a participant's configuration
	///
	/// Note if you provide an invalid participant ID, the returned information will be invalid (-1).
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID for the room the participant is in
	/// @param[in] participant_id The ID you'd like to get information about
	/// @param[out] config The configuration of the given participant. If you provided an invalid conference or participant ID, the returned configuration will be -1 for all values.
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_get_participant_configuration(imm_handle handle, int room_id, int participant_id, imm_participant_configuration* config);

	/// @brief Function to input audio data from a participant into a room
	///
	/// Every participant in a room should use this function to provide their individual inputs to the room.
	/// Later when calling the output audio function, the different inputs will be mixed according to the various settings.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID for the room you'd like to add the participant's audio to.
	/// @param[in] participant_id The ID you'd like to input audio for
	/// @param[in] audio_data The buffer that has the audio data you'd like to input stored in it.
	/// @param[in] number_frames The number of frames allocated in the audio_data buffer. Note number of frames is a channel insensitive value as described earlier.
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_input_audio_short(imm_handle handle, int room_id, int participant_id, const short* audio_data, int number_frames);

	/// @brief Duplicate of imm_input_audio_short except the input is a 32 bit float. See imm_input_audio_short for more details
	IMMERSITECH_API imm_error_code imm_input_audio_float(imm_handle handle, int room_id, int participant_id, const float* audio_data, int number_frames);

	/// @brief Duplicate of imm_input_audio_short except the input is a 64 bit float. See imm_input_audio_short for more details
	IMMERSITECH_API imm_error_code imm_input_audio_double(imm_handle handle, int room_id, int participant_id, const double* audio_data, int number_frames);

	/// @brief Function to process audio data and return the output audio data for a participant
	///
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID for the room corresponding to the participant whose output you'd like to generate.
	/// @param[in] participant_id The ID of the participant whose output stream will be generated. In other words, this is the output data this participant will hear.
	/// @param[out] output This is an audio data buffer that you have initialized. The output data will be written to this buffer. Ensure its allocated size matches the output number of frames and channels specified upon initializing the library.
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_output_audio_short(imm_handle handle, int room_id, int participant_id, short* output);

	/// @brief Duplicate of imm_output_audio_short except the output is a 32 bit float. See imm_output_audio_short for more details
	IMMERSITECH_API imm_error_code imm_output_audio_float(imm_handle handle, int room_id, int participant_id, float* output);

	/// @brief Duplicate of imm_output_audio_short except the output is a 64 bit float. See imm_output_audio_short for more details
	IMMERSITECH_API imm_error_code imm_output_audio_double(imm_handle handle, int room_id, int participant_id, double* output);

	/// @brief Function to set the x,y,z position of a given participant
	///
	/// Change both the position and heading of a participant. These two values fully quanitify the participant's 3D experience.
	/// Note that this function will only work for a room that is in open room mode.
	/// If not, first use imm_set_room_layout() to get the room to an open room layout, and then this function will work properly.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID for the room the participant is in
	/// @param[in] participant_id The ID of the participant you are relocating.
	/// @param[in] position The position you would like to move this participant to.
	/// @param[in] heading The direction you would like this participant to face.
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_set_participant_position(imm_handle handle, int room_id, int participant_id, imm_position position, imm_heading heading);

	/// @brief Function to retrieve Location of a given participant
	///
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID for the room the participant is in
	/// @param[in] participant_id The ID of the participant you want to know the position of
	/// @param[out] position A pointer to an imm_position object that you allocated. The position information you requested will be stored in this object.
	/// @param[out] heading A pointer to an imm_heading object that you allocated. The heading information you requested will be stored in this object.
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_get_participant_position(imm_handle handle, int room_id, int participant_id, imm_position* position, imm_heading* heading);

	/// @brief Function to set the state of a given participant
	///
	/// Use this function to adjust the various audio effects available through imm_audio_control
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID for the room the participant is in
	/// @param[in] participant_id The ID of the participant you want to adjust the settings for
	/// @param[in] control_to_edit The control you want to adjust.
	/// @param[in] value The new value of the control_to_edit that was specified.
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_set_participant_state(imm_handle handle, int room_id, int participant_id, imm_audio_control control_to_edit, int value);

	/// @brief Function to retrieve state of a given participant
	/// 
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID for the room the participant is in
	/// @param[in] participant_id The ID of the participant you want to view the settings for
	/// @param[in] control The control you want to learn the state of.
	/// @param[out] value If the call is a success, value is the value of the control of the requested participant in the requested conference.
	/// @return An imm_error_code value specifying whether or not the call was a success 
	IMMERSITECH_API imm_error_code imm_get_participant_state(imm_handle handle, int room_id, int participant_id, imm_audio_control control, int* value);

	/// @brief Function to retrieve Spherical Coordinates of a given source of a given participant
	///
	/// This function can give detailed information about the current 3D rendering between two participants.
	/// The returned angles are the spherical coordinates that represent where the listner (listener_ID) will hear the source (source_ID) rendered.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID for the room the participants are in
	/// @param[in] listener_id The ID of the participant who is listening to the source.
	/// @param[in] source_id The ID of the participant who is generated the sound heard by the listener.
	/// @param[out] azimuth The azimuth angle from the source to the listener.
	/// @param[out] elevation The elevation angle from the source to the listener.
	/// @param[out] distance The distance from the source to the listener.
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_get_participant_spherical(imm_handle handle, int room_id, int listener_id, int source_id, int* azimuth, int* elevation, int* distance);

	/// @brief Function to clear all data related to a particular participant
	///
	/// This function will remove all audio data that was input for this participant.
	/// It will also clear all data that was on the output path to this participant.
	/// @param[in] handle Your imm_handle given to you after successfully intializing the library with imm_initialize_library
	/// @param[in] room_id The ID for the room the participant is in
	/// @param[in] participant_id The ID of the participant whose data you would like to flush.
	/// @return An imm_error_code value specifying whether or not the call was a success
	IMMERSITECH_API imm_error_code imm_flush_data(imm_handle handle, int room_id, int participant_id);

	/// @brief Convert an imm_audio_control to a human-readable string description
	///
	/// For example, instead of viewing an enum value such as 3, you may rather see "IMM_CONTROL_AGC_ENABLE"
	/// @param[in] control The audio control enum value
	/// @return A string with the human readable version of the control
	IMMERSITECH_API const char* imm_audio_control_to_string(imm_audio_control control);

	/// @brief Convert string to an imm_audio_control enum value that is compatible with various sound manager functions
	///
	/// Convert a string to an actual enum control value to use in various functions.
	/// For example you may have "IMM_CONTROL_ANC_ENABLE" but you need an imm_audio_control enum value to send into a function
	/// @param[in] control_string The string representing the corresponding imm_audio_control value
	/// @return The enum value of the control
	IMMERSITECH_API imm_audio_control imm_string_to_audio_control(const char* control_string);

	/// @brief Convert string to specific participant value.
	///
	/// To edit an immersitech audio control you will need to have the value as an integer.
	/// If you have obtained the value as a string such as IMM_DEVICE_HEADPHONES or a boolean such as True, this function will convert to an integer
	/// @param[in] value_string The string representing the value you'd like to use
	/// @return The int corresponding to the input string
	IMMERSITECH_API int imm_string_to_participant_value(const char* value_string);

	/// @brief Get a human readable string for an error code
	///
	/// This function allows you to print an error code enum value as its human readable equivalent by returning a string
	/// @param[in] error_code The error code you'd like to get human readable form for
	/// @return A string representing the corresponding error code enum value
	IMMERSITECH_API const char* imm_error_code_to_string(imm_error_code error_code);

#ifdef __cplusplus
}
#endif

#endif // IMMERSITECH_LIBRARY_H_