#ifndef CHANNELS_H
#define CHANNELS_H

/* workaround for c-based functions according to 
 http://stackoverflow.com/questions/19635007/arduino-c-undefined-reference-to-readarms */
#ifdef __cplusplus
extern "C" {
#endif

/**
 Functions for managing channels of process data. Channels are the central point, where functionality
 is connected. Modules can either be invoked if a channels value is updated or they can update channel
 values themselves.

 Furthermore, channel content can also be streamed to external serial devices on to storage to implement a kind
 of data logger.

 Specific channels might be the current timestamp, an input voltage or an outgoing control value

 with the current implementation, 64 channels can be supported.
 */

/**
 * The maximum amount of channels which needs to be supported by this implementation
 */
#define CHANNEL_MAX_CHANNELS 32

/**
 * The maximum amount of modules which can be registered
 */
#define CHANNEL_MAX_MODULES 32

/**
 * Value to signal that no time prescale should be applied
 */
#define CHANNEL_NO_TIMEPRESCALE          -1

/**
 * Value to signal that no execution prescale should be applied
 */
#define CHANNEL_NO_EXECUTIONPRESCALE     -1

/**
 * ChannelId is used to uniquely identify channels
 */
typedef unsigned char ChannelId;

/**
 * ChannelMask is a bit mask to select a set of channels
 */
typedef unsigned long ChannelMask;

/**
 * This signature needs to be implemented by modules so the channels implementation
 * can call them on changed channel values.
 */
typedef int (*ChannelModuleExecute)(void*);

/* using http://www.arduino.cc/en/Reference/Micros, will overflow after 70 minutes */
/* must be long because this can be a time delta */
typedef long ChannelTime;

/**
 * The data type hold by one channel
 */
enum ChannelDataType {
	ChannelDataType_Unused = 1,
	ChannelDataType_Float,
	ChannelDataType_Int,
	ChannelDataType_ULong
};

/**
 * Needs to be called before any other method is used to initialize internal
 * data structures.
 */
void channel_initialize();

/**
 * Add a new channel to the channels system. The ChannelId must be smaller than
 * CHANNEL_MAX_CHANNELS and must not be used before.
 */
void channel_add(ChannelId, enum ChannelDataType);

/**
 * Return the data type used by one channel.
 */
enum ChannelDataType channel_get_data_type(ChannelId);

/**
 * Register a module in the channels system. The first parameter is the call back
 * method used to execute the module code. The ChannelMask is a bitmask, which is true for all channel
 * numbers where the module should be executed if the channel value changes.
 * The ChannelTime is the minimum time which is waited before the module is executed a second time.
 * The integer parameter is the execution pre-scale, meaning the number of calls to modules execute which
 * are skipped before actually calling the module.
 * Both the time and the execution pre-scale are used to limit the execution rate of a module. They can not
 * be combined and CHANNEL_NO_TIMEPRESCALE and CHANNEL_NO_EXECUTIONPRESCALE can be used as a parameter
 * in order to disable them.
 */
void channel_register_module(ChannelModuleExecute, void *, ChannelMask,
		ChannelTime, int);

/**
 * Sets the value of a float channel
 */
void channel_set_value_f(ChannelId, float);

/**
 * Returns the value of a float channel
 */
float channel_get_value_f(ChannelId);

/**
 * Sets the value of a unsigned long channel
 */
void channel_set_value_ul(ChannelId, unsigned long);

/**
 * Returns the value of an unsigned long channel
 */
unsigned long channel_get_value_ul(ChannelId);

/**
 * Sets the value of an integer channel
 */
void channel_set_value_i(ChannelId, int);

/**
 * Returns the value of an integer channel
 */
int channel_get_value_i(ChannelId);

/**
 step, will execute as long as modules still change values
 of channels*/
void channel_step(ChannelTime);

/**
 * Release all internal data structures
 */
void channel_release();

#ifdef __cplusplus
}
#endif

#endif

