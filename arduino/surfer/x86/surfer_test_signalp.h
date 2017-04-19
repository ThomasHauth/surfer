#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"

#include "../Channels.h"
#include "../SignalProcess.h"

int g_oneSecond = 1000000;

void test_signalp_smoother_rectangle() {
	ChannelId testChannelOne = 1;
	ChannelId testChannelTwo = 2;
	int i;

	channel_initialize();
	channel_add(testChannelOne, ChannelDataType_Float);
	channel_add(testChannelTwo, ChannelDataType_Float);

	struct signalp_smoother_parameter parameter;

	parameter.input_channel_id = testChannelOne;
	parameter.output_channel_id = testChannelTwo;
	parameter.window_size = 4;
	parameter.algorithm = SignalSmoothingAlgorithm_Rectangular;

	signalp_smoother_initialize(&parameter);

	/* looking for change in ValueOne*/
	channel_register_module(&signalp_smoother, &parameter, 2,
	CHANNEL_NO_TIMEPRESCALE,
	CHANNEL_NO_EXECUTIONPRESCALE);

	channel_set_value_f(testChannelOne, 10.0f);
	channel_step(1.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelTwo), 10.0f, 0.001f);

	channel_set_value_f(testChannelOne, 20.0f);
	channel_step(1.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelTwo), 12.5f, 0.001f);

	channel_set_value_f(testChannelOne, 20.0f);
	channel_step(1.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelTwo), 15.0f, 0.1f);

	channel_set_value_f(testChannelOne, 10.0f);
	channel_step(1.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelTwo), 15.0f, 0.01f);

	channel_set_value_f(testChannelOne, 40.0f);
	channel_step(1.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelTwo), 22.5f, 0.01f);

	for (i = 0; i < 400; i++) {
		channel_set_value_f(testChannelOne, -40.0f);
		channel_step(1.0f);
	}
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelTwo), -40.0f, 0.001f);

	channel_release();
}

void test_signalp_smoother_triangle() {
	ChannelId testChannelOne = 1;
	ChannelId testChannelTwo = 2;

	channel_initialize();
	channel_add(testChannelOne, ChannelDataType_Float);
	channel_add(testChannelTwo, ChannelDataType_Float);

	struct signalp_smoother_parameter parameter;

	parameter.input_channel_id = testChannelOne;
	parameter.output_channel_id = testChannelTwo;
	parameter.window_size = 4;
	parameter.algorithm = SignalSmoothingAlgorithm_Triangle;

	signalp_smoother_initialize(&parameter);

	/* looking for change in ValueOne*/
	channel_register_module(&signalp_smoother, &parameter, 2,
	CHANNEL_NO_TIMEPRESCALE,
	CHANNEL_NO_EXECUTIONPRESCALE);

	channel_set_value_f(testChannelOne, 10.0f);
	channel_step(1.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelTwo), 10.0f, 0.001f);

	channel_set_value_f(testChannelOne, 40.0f);
	channel_step(1.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelTwo), 22.0f, 0.001f);
	/*
	 for ( i = 0; i < 400; i++ ) {
	 channel_set_value_f(testChannelOne, -40.0f);
	 channel_step(1.0f);
	 }
	 CU_ASSERT_DOUBLE_EQUAL( channel_get_value_f( testChannelTwo ), -40.0f, 0.001f );
	 */
	channel_release();
}

void test_signalp_smooth_control_growing_slowing() {
	ChannelId testChannelSet = 1;
	ChannelId testChannelActual = 2;

	ChannelId testChannelAccelMin = 3;
	ChannelId testChannelAccelMax = 4;
	ChannelId testChannelAccelExponent = 5;

	channel_initialize();
	channel_add(testChannelSet, ChannelDataType_Float);
	channel_add(testChannelActual, ChannelDataType_Float);

	channel_add(testChannelAccelMin, ChannelDataType_Float);
	channel_add(testChannelAccelMax, ChannelDataType_Float);
	channel_add(testChannelAccelExponent, ChannelDataType_Float);

	channel_set_value_f(testChannelSet, 0.0f);
	channel_set_value_f(testChannelActual, 0.0f);

	channel_set_value_f(testChannelAccelMin, 0.5f);
	channel_set_value_f(testChannelAccelMax, 0.5f);
	channel_set_value_f(testChannelAccelExponent, 1.0f);

	struct signalp_smooth_control_parameter parameter;

	parameter.set_channel_id = testChannelSet;
	parameter.actual_channel_id = testChannelActual;
	parameter.accel_min_channel_id = testChannelAccelMin;
	parameter.accel_max_channel_id = testChannelAccelMax;
	parameter.accel_exponent_channel_id = testChannelAccelExponent;
	// in seconds , update every 100 ms
	parameter.control_time = 1.0f;

	// watch for changes in the set channel and the timing channel
	channel_register_module(&signalp_smooth_control, &parameter, 1,
	// only call every 100ms
			1000.0f * 100.0f,
			CHANNEL_NO_EXECUTIONPRESCALE);

	// should take 5 seconds to reach
	channel_set_value_f(testChannelSet, 1.0f);

	channel_step(1020.0f * 1000.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelActual), 0.5f, 0.001f);
	channel_step(1020.0f * 1000.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelActual), 1.0f, 0.001f);

	// slow down ...
	channel_set_value_f(testChannelSet, 0.0f);

	channel_step(1020.0f * 1000.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelActual), 0.5f, 0.001f);
	channel_step(1020.0f * 1000.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelActual), 0.0f, 0.001f);
}

void test_signalp_smooth_control_growing_exponent() {
	ChannelId testChannelSet = 1;
	ChannelId testChannelActual = 2;

	ChannelId testChannelAccelMin = 3;
	ChannelId testChannelAccelMax = 4;
	ChannelId testChannelAccelExponent = 5;

	channel_initialize();
	channel_add(testChannelSet, ChannelDataType_Float);
	channel_add(testChannelActual, ChannelDataType_Float);

	channel_add(testChannelAccelMin, ChannelDataType_Float);
	channel_add(testChannelAccelMax, ChannelDataType_Float);
	channel_add(testChannelAccelExponent, ChannelDataType_Float);

	channel_set_value_f(testChannelSet, 0.0f);
	channel_set_value_f(testChannelActual, 0.0f);

	channel_set_value_f(testChannelAccelMin, 0.1f);
	channel_set_value_f(testChannelAccelMax, 0.5f);
	channel_set_value_f(testChannelAccelExponent, 3.0f);

	struct signalp_smooth_control_parameter parameter;

	parameter.set_channel_id = testChannelSet;
	parameter.actual_channel_id = testChannelActual;
	parameter.accel_min_channel_id = testChannelAccelMin;
	parameter.accel_max_channel_id = testChannelAccelMax;
	parameter.accel_exponent_channel_id = testChannelAccelExponent;
	// in seconds , update every 100 ms
	parameter.control_time = 1.0f;

	// watch for changes in the set channel and the timing channel
	channel_register_module(&signalp_smooth_control, &parameter, 1,
	// only call every 100ms
			1000.0f * 100.0f,
			CHANNEL_NO_EXECUTIONPRESCALE);

	// should take 5 seconds to reach
	channel_set_value_f(testChannelSet, 1.0f);

	channel_step(1020.0f * 1000.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelActual), 0.1f, 0.001f);
	channel_step(1020.0f * 1000.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelActual), 0.2f, 0.001f);

	channel_step(1020.0f * 1000.0f);
	channel_step(1020.0f * 1000.0f);
	channel_step(1020.0f * 1000.0f);
	channel_step(1020.0f * 1000.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelActual), 0.707f,
			0.001f);
}

void test_signalp_smooth_control() {
	ChannelId testChannelSet = 1;
	ChannelId testChannelActual = 2;

	ChannelId testChannelAccelMin = 3;
	ChannelId testChannelAccelMax = 4;
	ChannelId testChannelAccelExponent = 5;

	channel_initialize();
	channel_add(testChannelSet, ChannelDataType_Float);
	channel_add(testChannelActual, ChannelDataType_Float);

	channel_add(testChannelAccelMin, ChannelDataType_Float);
	channel_add(testChannelAccelMax, ChannelDataType_Float);
	channel_add(testChannelAccelExponent, ChannelDataType_Float);

	channel_set_value_f(testChannelSet, 0.0f);
	channel_set_value_f(testChannelActual, 0.0f);

	channel_set_value_f(testChannelAccelMin, 0.2f);
	channel_set_value_f(testChannelAccelMax, 0.2f);
	channel_set_value_f(testChannelAccelExponent, 1.0f);

	struct signalp_smooth_control_parameter parameter;

	parameter.set_channel_id = testChannelSet;
	parameter.actual_channel_id = testChannelActual;
	parameter.accel_min_channel_id = testChannelAccelMin;
	parameter.accel_max_channel_id = testChannelAccelMax;
	parameter.accel_exponent_channel_id = testChannelAccelExponent;
	// in seconds , update every 100 ms
	parameter.control_time = 0.1f;

	// watch for changes in the set channel and the timing channel
	channel_register_module(&signalp_smooth_control, &parameter, 1,
	// only call every 100ms
			1000.0f * 100.0f,
			CHANNEL_NO_EXECUTIONPRESCALE);

	// nothing should happen if the set value is 0
	channel_step(50.0f * 1000.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelActual), 0.0f, 0.001f);

	channel_set_value_f(testChannelSet, 0.0f);
	channel_step(120.0f * 1000.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelActual), 0.0f, 0.001f);

	// should take 5 seconds to reach
	channel_set_value_f(testChannelSet, 1.0f);
	channel_step(120.0f * 1000.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelActual), 0.02f,
			0.001f);

	channel_step(120.0f * 1000.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelActual), 0.04f,
			0.001f);

	channel_step(120.0f * 1000.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelActual), 0.06f,
			0.001f);
}

void test_signalp_subtract() {
	ChannelId testChannelOne = 1;
	ChannelId testChannelTwo = 2;
	ChannelId testChannelResult = 3;

	channel_initialize();
	channel_add(testChannelOne, ChannelDataType_Float);
	channel_add(testChannelTwo, ChannelDataType_Float);
	channel_add(testChannelResult, ChannelDataType_Float);

	struct signalp_substract_parameter parameter;

	parameter.input_channel_a_id = testChannelOne;
	parameter.input_channel_b_id = testChannelTwo;
	parameter.output_channel_id = testChannelResult;

	channel_set_value_f(testChannelOne, 10.0f);
	channel_set_value_f(testChannelTwo, 10.0f);

	// watch for changes in both Channle testChannelOne and testChannelTwo
	channel_register_module(&signalp_substract, &parameter, 2 | 4,
	CHANNEL_NO_TIMEPRESCALE,
	CHANNEL_NO_EXECUTIONPRESCALE);

	channel_set_value_f(testChannelOne, 15.0f);
	channel_step(1.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelResult), 5.0f, 0.001f);

	channel_set_value_f(testChannelTwo, 5.0f);
	channel_step(1.0f);
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannelResult), 10.0f,
			0.001f);

	channel_release();
}
