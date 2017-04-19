#include "StreamChannel.h"

#include "Logging.h"
#include "SerialInterface.h"

struct stream_setting {
	int setting;
};

struct stream_setting stream_settings[CHANNEL_MAX_CHANNELS];

void stream_transmit_channel_ex(ChannelId channel_id) {
	int i;
	float f;
	enum ChannelDataType ct = channel_get_data_type(channel_id);

	struct SerialMessage * msg = ser_allocate(SerialMessageType_OUTPUT,
			channel_id);

	switch (ct) {
	case ChannelDataType_Float:
		msg->m_payloadType = SerialMessagePayload_Float;
		f = channel_get_value_f(channel_id);
		msg->m_payloadFloat = f;
		break;
	case ChannelDataType_Int:
		msg->m_payloadType = SerialMessagePayload_Int;
		i = channel_get_value_i(channel_id);
		msg->m_payloadInt = i;
		break;
	case ChannelDataType_ULong:
		msg->m_payloadType = SerialMessagePayload_Int;
		// this will suffice for the programs usecase
		i = (int) channel_get_value_ul(channel_id);
		msg->m_payloadInt = i;
		break;
	default:
		LOG_ERROR("Data type %i not supportd by stream channel\n", ct)
		;
	}

	ser_transmit(msg);
	ser_free_messages(msg);

}

int stream_transmit_channel(void * parameter) {
	struct stream_transmit_parameter * s_parameter =
			(struct stream_transmit_parameter *) parameter;

	stream_transmit_channel_ex(s_parameter->channel_id);

	return 0;
}

void stream_channel(ChannelId channel_id, int setting) {
	stream_settings[channel_id].setting = setting;
}

long stream_sum_dt;

void stream_initialize() {
	int i;
	for (i = 0; i < CHANNEL_MAX_CHANNELS; i++) {
		stream_settings[i].setting = STREAM_DISABLE;
	}
}

void stream_subscribed_channels(long dt) {
	stream_sum_dt += dt;
	int i;

	// only stream every second
	if (stream_sum_dt > 1000000) {
		for (i = 0; i < CHANNEL_MAX_CHANNELS; i++) {
			if (stream_settings[i].setting > 0) {
				stream_transmit_channel_ex(i);
			}
		}
		stream_sum_dt = 0;
	}
}

