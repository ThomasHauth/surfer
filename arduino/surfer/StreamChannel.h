#ifndef STREAMCHANNEL_H
#define STREAMCHANNEL_H

#include "Channels.h"

/* workaround for c-based functions according to 
 http://stackoverflow.com/questions/19635007/arduino-c-undefined-reference-to-readarms */
#ifdef __cplusplus
extern "C" {
#endif

struct stream_transmit_parameter {
	ChannelId channel_id;
};

#define STREAM_DISABLE   -1
#define STREAM_ONCHANGE  -2

int stream_transmit_channel(void *);

/*
 can have options for maximum rate
 or only if changed later
 */
void stream_channel(ChannelId, int);

void stream_subscribed_channels(long);

#ifdef __cplusplus
}
#endif

#endif

