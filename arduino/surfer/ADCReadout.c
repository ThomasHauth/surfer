#include "ADCReadout.h"

#include "Logging.h"

#include <Arduino.h>

void adcreadout_initialize(struct adcreadout_parameter * p) {
	p->float_transform = 0;
}

int adcreadout_execute(void * parameter) {
	struct adcreadout_parameter * casted_parameter =
			(struct adcreadout_parameter*) parameter;

	int val = analogRead(casted_parameter->pin);
	float fVal = 0.0f;

	// check which channel formal we have to write
	enum ChannelDataType ct = channel_get_data_type(
			casted_parameter->write_channel_id);
	switch (ct) {
	case ChannelDataType_Float:
		fVal = (float) val;
		if (casted_parameter->float_transform > 0) {
			// transform to indicated range
			//LOG_ERROR("fVal is %i\n", val);
			fVal =
					casted_parameter->float_min
							+ (casted_parameter->float_max
									- casted_parameter->float_min)
									* (fVal / 1024.0);
		}

		channel_set_value_f(casted_parameter->write_channel_id, fVal);
		break;
	case ChannelDataType_Int:
		channel_set_value_i(casted_parameter->write_channel_id, val);
		break;
	default:
		LOG_ERROR("Data type %i not supportd by adc readoutl\n", ct)
		;
	}

	return 0;
}

