#include "ConsoleIO.h"

#include "../Channels.h"
#include "../Logging.h"

int consoleio_dump_float_channel(void * parameter) {
	struct consoleio_parameter * c_parameter =
			(struct consoleio_parameter *) parameter;

	float v = channel_get_value_f(c_parameter->read_channel_id);
	LOG_INFO(">> Channel %i has float value %f\n", c_parameter->read_channel_id,
			v);
	return 0;
}
