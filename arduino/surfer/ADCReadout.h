#ifndef ADC_READOUT
#define ADC_READOUT

#include "Channels.h"

struct adcreadout_parameter {
	int pin;
	ChannelId write_channel_id;
	char float_transform;
	float float_min;
	float float_max;
};

void adcreadout_initialize(struct adcreadout_parameter *);
int adcreadout_execute(void*);

#endif
