
#include "../Channels.h"

#include "ConsoleIO.h"

#include <stdio.h>
#include <sys/time.h>

#include <unistd.h>

/**
 * This simple setup simulates two channels and executes
 * 1000 iterations for testing purposes.
 */

enum SurferChannelId {
	// channel id 0 is reserved for time !
	SurferChannelId_ValueOne = 1,
	SurferChannelId_ValueTwo = 2
};

int main() {
	int i = 0;

	printf("Surfer stack for x86\n");

	channel_add(SurferChannelId_ValueOne, ChannelDataType_Float);

	/*channel_register_module( &adcreadout_execute, &aparam, 2,
	 CHANNEL_NO_TIMEPRESCALE,
	 CHANNEL_NO_EXECUTIONPRESCALE);
	 channel_register_module( &adcreadout_execute, &aparam2, 4,
	 CHANNEL_NO_TIMEPRESCALE,
	 CHANNEL_NO_EXECUTIONPRESCALE);*/

	struct consoleio_parameter fdump;
	fdump.read_channel_id = SurferChannelId_ValueOne;
	channel_register_module(&consoleio_dump_float_channel, &fdump, 1, 10000,
	CHANNEL_NO_EXECUTIONPRESCALE);

	for (; i < 2000; i++) {
		channel_set_value_f(SurferChannelId_ValueOne, 33.3f);

		if ((i % 2) == 0) {
			channel_set_value_f(SurferChannelId_ValueTwo, 5.3f);

		}

		/* simulation 1000 us = 1 ms steps here */
		channel_step(1000);
	}

	return 0;
}

