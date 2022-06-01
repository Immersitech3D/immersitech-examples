#pragma once

#ifndef IMM_EVENT_MANAGER_H_
#define IMM_EVENT_MANAGER_H_

#include "immersitech.h"

/// @brief All possible events that an Immersitech Event manager can support
///
typedef enum {
	IMM_EVENT_CREATE_ROOM,					///< An event triggered when a room is created
	IMM_EVENT_DESTROY_ROOM,					///< An event triggered when a room is destroyed
	IMM_EVENT_ADD_PARTICIPANT,				///< An event triggered when a participant is added to a room
	IMM_EVENT_REMOVE_PARTICIPANT,			///< An event triggered when a participant is removed from a room
	IMM_EVENT_SET_PARTICIPANT_SEAT,			///< An event triggered when a participant changes seats
	IMM_EVENT_SET_PARTICIPANT_POSITION,		///< An event triggered when a participant changes position
	IMM_EVENT_SET_PARTICIPANT_STATE,		///< An event triggered when a participant changes the value of an audio control
	IMM_EVENT_SET_ALL_PARTICIPANTS_STATE,	///< An event triggered when all participants change the value of an audio control
	IMM_EVENT_SET_ROOM_LAYOUT,				///< An event triggered when a room has its layout changed
	IMM_EVENT_SET_NAME						///< An event triggered when a participant's name changes
} imm_event_type;

/// @brief Default immersitech event manager.
///
/// The default event manager will not do anything upon function call as seen in the function definitions.
/// This class is made for a developer to inherit and create a custom event manager.
/// With a custom event manager, each function can perform actions specific to their system / application.
class imm_event_manager {
public:

	/// @brief Create a default event manager
	///
	/// In your custom event manager, you can set up any member variables here
	imm_event_manager() {}

	/// @brief Destroy a default event manager
	///
	/// In your custom event manager, you can destroy any member variables you set up in the constructor
	virtual ~imm_event_manager() {}

	/// @brief A function to propagate an IMM_EVENT_CREATE_ROOM event
	///
	/// In your custom event manager, implement this function to define what you would like to happen upon this event type being triggered.
	/// @param[in] room_id The ID of the room that was created
	virtual void create_room_event(int room_id) {}

	/// @brief A function to propagate an IMM_EVENT_DESTROY_ROOM event
	///
	/// In your custom event manager, implement this function to define what you would like to happen upon this event type being triggered.
	/// @param[in] room_id The ID of the room that was destroyed
	virtual void destroy_room_event(int room_id) {}

	/// @brief A function to propagate an IMM_EVENT_ADD_PARTICIPANT event
	///
	/// In your custom event manager, implement this function to define what you would like to happen upon this event type being triggered.
	/// @param[in] room_id The ID of the room that the participant was added to
	/// @param[in] participant_id The ID of the participant that was added
	/// @param[in] participant_name The name of the participant that was added
	/// @param[in] config The configuration for the participant such as input sampling rate and input number of frames
	virtual void add_participant_event(int room_id, int participant_id, const char* participant_name, imm_participant_configuration config) {}

	/// @brief A function to propagate an IMM_EVENT_REMOVE_PARTICIPANT event
	///
	/// In your custom event manager, implement this function to define what you would like to happen upon this event type being triggered.
	/// @param[in] room_id The ID of the room that the participant was removed from
	/// @param[in] participant_id The ID of the participant that was removed
	virtual void remove_participant_event(int room_id, int participant_id) {}

	/// @brief A function to propagate an IMM_EVENT_SET_PARTICIPANT_SEAT event
	///
	/// In your custom event manager, implement this function to define what you would like to happen upon this event type being triggered.
	/// @param[in] room_id The ID of the room that the participant was moved in
	/// @param[in] participant_id The ID of the participant that was moved
	/// @param[in] seat The seat that the participant was moved to
	virtual void set_participant_seat_event(int room_id, int participant_id, imm_seat seat) {}

	/// @brief A function to propagate an IMM_EVENT_SET_PARTICIPANT_POSITION event
	///
	/// In your custom event manager, implement this function to define what you would like to happen upon this event type being triggered.
	/// @param[in] room_id The ID of the room that the participant was moved in
	/// @param[in] participant_id The ID of the participant that was moved
	/// @param[in] position The new position of the participant
	/// @param[in] heading The new heading of the participant
	virtual void set_participant_position_event(int room_id, int participant_id, imm_position position, imm_heading heading) {}

	/// @brief A function to propagate an IMM_EVENT_SET_PARTICIPANT_STATE event
	///
	/// In your custom event manager, implement this function to define what you would like to happen upon this event type being triggered.
	/// @param[in] room_id The ID of the room that the participant is in
	/// @param[in] participant_id The ID of the participant whose state was changed
	/// @param[in] control_to_edit The audio control the was changed
	/// @param[in] value The value the audio control was set to
	virtual void set_participant_state_event(int room_id, int participant_id, imm_audio_control control_to_edit, int value) {}

	/// @brief A function to propagate an IMM_EVENT_SET_ALL_PARTICIPANTS_STATE event
	///
	/// In your custom event manager, implement this function to define what you would like to happen upon this event type being triggered.
	/// @param[in] room_id The ID of the room that the participants are in
	/// @param[in] control_to_edit The audio control that was changed
	/// @param[in] value The value the audio control was set to
	virtual void set_all_participants_state_event(int room_id, imm_audio_control control_to_edit, int value) {}

	/// @brief A function to propagate an IMM_EVENT_SET_ROOM_LAYOUT event
	///
	/// In your custom event manager, implement this function to define what you would like to happen upon this event type being triggered.
	/// @param[in] room_id The ID of the room that changed its layout
	/// @param[in] room_layout_id The ID of the room layout that was applied
	virtual void set_room_layout_event(int room_id, int room_layout_id) {}

	/// @brief A function to propagate an IMM_EVENT_SET_NAME event
	///
	/// In your custom event manager, implement this function to define what you would like to happen upon this event type being triggered.
	/// @param[in] room_id The ID of the room that changed its layout
	/// @param[in] participant_id The ID of the participant whose name changed
	/// @param[in] name The new name for the participant
	virtual void set_name_event(int room_id, int participant_id, const char* name) {}

private:
};

#endif //IMM_EVENT_MANAGER_H_