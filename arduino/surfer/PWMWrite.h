#ifndef PWM_WRITE
#define PWM_WRITE

#include "Channels.h"

struct pwmwrite_parameter {
	int pin;
	ChannelId read_channel_id;
};

int pwmwrite_execute(void*);

#endif
