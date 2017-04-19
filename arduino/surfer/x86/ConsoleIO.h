#ifndef CONSOLEIO_H
#define CONSOLEIO_H

#include "../Channels.h"

struct consoleio_parameter {
	ChannelId read_channel_id;
};

int consoleio_dump_float_channel(void*);

#endif
