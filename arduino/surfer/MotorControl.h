#ifndef MOTOR_CONTROL
#define MOTOR_CONTROL

#include <Arduino.h>

#include "Channels.h"

struct motor_set_parameter {
	int pin;
	ChannelId read_channel_id;

	Servo * _servo;
};

int motor_set_initialize(motor_set_parameter * p) {
	p->_servo = new Servo();
	p->_servo->attach(p->pin, 1000, 2000); /* parameters are lower/upper bound in us */
	LOG_ERROR("Attaching Motor 1 control on pin %i\n", p->pin);

	// neutral
	p->_servo->writeMicroseconds(1500);
	// this is supposed to enable the programming mode of the motor controller
	/* delay(100);
	 p->_servo->writeMicroseconds(2000);
	 delay(50);
	 p->_servo->writeMicroseconds(1000);
	 delay(50);
	 p->_servo->writeMicroseconds(1500);
	 delay(100);
	 p->_servo->writeMicroseconds(2000);
	 delay(100);
	 p->_servo->writeMicroseconds(1500);
	 delay(100);
	 */
	LOG_ERROR("Motor initialized\n");
}

int motor_set_execute(void* p) {

	motor_set_parameter * mp = (motor_set_parameter *) p;

	// check which channel formal we have to read
	enum ChannelDataType ct = channel_get_data_type(mp->read_channel_id);
	float fv = 0.0f;

	switch (ct) {
	case ChannelDataType_Float:
		fv = channel_get_value_f(mp->read_channel_id);
		LOG_ERROR("Motor: Setting serial val %f\n", fv);
		//mp->_servo->writeMicroseconds( 1500.0f + (500.0f * fv) );
		// with the current setup of the motor control, only the 1500 to 2000 range can be used for forward movement
		mp->_servo->writeMicroseconds(1500.0f + (500.0f * fv));
		break;
	default:
		LOG_ERROR("Data type %i not supportd by motor control\n", ct);
	}

	return 0;
}

#endif
