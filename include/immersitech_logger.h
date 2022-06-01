#pragma once

#ifndef IMM_LOGGER_H_
#define IMM_LOGGER_H_

#if HAVE_VISIBILITY
#define IMMERSITECH_LOGGER_API __attribute__((__visibility__("default")))
#elif defined _WIN32
#define IMMERSITECH_LOGGER_API __declspec(dllexport)
#else
#define IMMERSITECH_LOGGER_API
#endif

#include <string>
#include <mutex>

/// @brief Log level definition.
///
/// A message written to the log can have one of these levels of severity
typedef enum {
	IMM_LOG_VERBOSE,	///< This level is used for anything, no matter how unimportant
	IMM_LOG_DEBUG,		///< This level should be used only during a debugging session. It should print information that is helpful to the debugging process 
	IMM_LOG_INFO,		///< This level can be used to print generic information. For example, a participant was added to a room
	IMM_LOG_WARNING,	///< This level should be used for something that happened that could lead to an error or unexpected behavior
	IMM_LOG_ERROR		///< This level is exclusively for critical errors that have affected functionality
} imm_log_level;

/// @brief Default Immersitech logger handler
///
/// If you would like to expand on the logging system, you can inhert this class to implement your own behavior specific to your system.
class IMMERSITECH_LOGGER_API imm_logger_handler {
public:
	/// @brief Construct a new immersitech logger handler object
	///
	imm_logger_handler();

	/// @brief Destroy the immersitech logger handler object
	///
	virtual ~imm_logger_handler();

	/// @brief This function will process a message and post it to the appropriate log
	///
	/// If you inherit this class to make a custom logger, this is the function that will handle most of the processing.
	/// @param[in] level The log level which this message should be printed with
	/// @param[in] str The message that is to be logged
	virtual void handle(imm_log_level level, const char* str);
};

/// @brief Logger class.
///
class IMMERSITECH_LOGGER_API imm_logger {
	/// @brief The pointer to the logging instance
	///
	static imm_logger* m_pThis;
public:

	/// @brief Destroy the logger object
	///
	virtual ~imm_logger();

	/// @brief Initialize logger with speficied handler.
	///
	void initialize(imm_logger_handler* handler);

	/// @brief Get the logger object
	///
	/// @return A pointer to the logger
	static imm_logger* get_logger();

	/// @brief write a message to a log
	///
	/// @param[in] level The log level which this message should be printed with 
	/// @param[in] format A character array that can be overload just like the printf() function. For example: log(IMM_LOG_DEBUG, "The number is %i for participant %s", number, name);
	virtual void log(imm_log_level level, const char* format, ...);

	/// @brief Convert log level to string.
	///
	/// @param[in] level log level as an enum value
	/// @return log leve as a string
	static std::string to_string(imm_log_level level);

	/// @brief Turn the logger on or off
	///
	/// @param[in] enable A boolean describing if you'd like the logger to be active
	void set_enabled(bool enable);

	/// @brief Set the log level
	///
	/// @param[in] level Current log level the logger passes to the log
	void set_log_level(imm_log_level level);

private:
	/// brief Construct a singleton logger object
	///
	imm_logger();

	/// @brief Pointer to the handler. 
	///
	/// The handler can point to an inherited custom handler
	imm_logger_handler* handler;

	/// @brief The log lock is used to ensure thread safety when using the logger
	///
	std::mutex _log_lock;

	/// @brief This boolean can be used to turn the logger on or off in real time
	///
	bool _enabled;

	/// @brief This keeps track of the current log level. Anything below this level will not be logged
	///
	imm_log_level _imm_log_level;
};

#ifndef IMM_LOGGER
#define IMM_LOGGER imm_logger::get_logger()
#endif

#endif //IMM_LOGGER_H_