#include <stdio.h>
#include <string.h>
#include <CUnit/Basic.h>

#include "../Channels.h"

#include "surfer_test_signalp.h"

/**
 * This is a set of unit tests to check the primary functionality
 * of the surfer code base
 */

/** The suite initialization function.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1(void) {
	return 0;
}

/** The suite cleanup function.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void) {
	return 0;
}

/**
 * Tests if a float value is properly stored and retrieved.
 */
void testChannelsSetGetValueF(void) {
	ChannelId testChannel = 1;
	float fVal = 30.0f;

	channel_initialize();
	channel_add(testChannel, ChannelDataType_Float);
	/* check if properly zero initialized */
	CU_ASSERT_DOUBLE_EQUAL(channel_get_value_f(testChannel), 0.0f, 0.001f);

	channel_set_value_f(testChannel, fVal);

	CU_ASSERT_EQUAL(fVal, channel_get_value_f(testChannel));
	channel_release();
}

/**
 * Module parameter which just holds the numbers of calls to the module.
 */
struct ModuleCheckCallParameter {
	unsigned int call_count;
};

/**
 * This is surfer module just for unit testing and simply
 * increases the call_count stored in the parameter struct.
 */
int testModuleCheckCall(void * parameter) {
	struct ModuleCheckCallParameter * m_parameter =
			(struct ModuleCheckCallParameter *) parameter;
	m_parameter->call_count++;
	return 0;
}

/**
 * This is the parameter struct forthe ModuleChangeChannel and holds
 * which channel should be changed and what the new value is.
 */
struct ModuleChangeChannel {
	ChannelId id;
	float new_value;
};

/**
 * Test module which changes the value of a specific channel to what
 * is configured in the ModuleChangeChannel parameter struct.
 */
int testModuleChangeChannel(void * parameter) {
	struct ModuleChangeChannel * m_parameter =
			(struct ModuleChangeChannel *) parameter;

	channel_set_value_f(m_parameter->id, m_parameter->new_value);
	return 0;
}

/**
 * Check if all modules registered on a channel are properly called
 * once the channel value changes.
 */
void testChannelChangeValueExecuteModule(void) {
	ChannelId testChannelOne = 1;
	ChannelId testChannelTwo = 2;
	float fVal = 30.0f;

	struct ModuleCheckCallParameter mod_parameter_one;
	struct ModuleCheckCallParameter mod_parameter_two;

	channel_initialize();
	channel_add(testChannelOne, ChannelDataType_Float);
	channel_add(testChannelTwo, ChannelDataType_Float);

	mod_parameter_one.call_count = 0;
	mod_parameter_two.call_count = 0;

	/* looking for change in ValueOne*/
	channel_register_module(&testModuleCheckCall, &mod_parameter_one,
	/* only register on changes of channel one*/
	testChannelOne,
	CHANNEL_NO_TIMEPRESCALE,
	CHANNEL_NO_EXECUTIONPRESCALE);
	/* looking for change in ValueTwo*/
	channel_register_module(&testModuleCheckCall, &mod_parameter_two,
	/* register on changes of both channels */
	testChannelOne | testChannelTwo,
	CHANNEL_NO_TIMEPRESCALE,
	CHANNEL_NO_EXECUTIONPRESCALE);

	channel_set_value_f(testChannelOne, fVal);
	channel_step(1.0f);

	CU_ASSERT_EQUAL(1, mod_parameter_one.call_count);
	CU_ASSERT_EQUAL(1, mod_parameter_two.call_count);

	channel_step(1.0f);

	CU_ASSERT_EQUAL(1, mod_parameter_one.call_count);
	CU_ASSERT_EQUAL(1, mod_parameter_two.call_count);

	channel_set_value_f(testChannelTwo, fVal);
	channel_step(1.0f);

	CU_ASSERT_EQUAL(1, mod_parameter_one.call_count);
	CU_ASSERT_EQUAL(2, mod_parameter_two.call_count);

	channel_release();
}

/**
 * Checks if a module is called which is registered via an indirect
 * way :
 * A->B->C
 * A is changed and the module is registered on change of C
 */
void testChannelChangeValueExecuteModuleChained(void) {
	ChannelId testChannelOne = 1;
	ChannelId testChannelTwo = 2;
	float fVal = 30.0f;

	struct ModuleCheckCallParameter mod_parameter_two;
	struct ModuleChangeChannel mod_parameter_change;

	channel_initialize();
	channel_add(testChannelOne, ChannelDataType_Float);
	channel_add(testChannelTwo, ChannelDataType_Float);

	mod_parameter_two.call_count = 0;
	mod_parameter_change.id = testChannelTwo;
	mod_parameter_change.new_value = 5.0f;

	channel_register_module(&testModuleChangeChannel, &mod_parameter_change, 2,
	CHANNEL_NO_TIMEPRESCALE,
	CHANNEL_NO_EXECUTIONPRESCALE);
	channel_register_module(&testModuleCheckCall, &mod_parameter_two, 4,
	CHANNEL_NO_TIMEPRESCALE,
	CHANNEL_NO_EXECUTIONPRESCALE);

	channel_set_value_f(testChannelOne, fVal);
	channel_step(1.0f);

	CU_ASSERT_EQUAL(1, mod_parameter_two.call_count);

	channel_set_value_f(testChannelOne, fVal + fVal);
	channel_step(1.0f);

	CU_ASSERT_EQUAL(2, mod_parameter_two.call_count);
	channel_release();
}

/**
 * Checks whether the execution pre-scaling of notifications works
 */
void testChannelChangeValueExecutionPrescale(void) {
	ChannelId testChannelOne = 1;
	float fVal = 30.0f;

	struct ModuleCheckCallParameter mod_parameter_one;

	channel_initialize();
	channel_add(testChannelOne, ChannelDataType_Float);

	mod_parameter_one.call_count = 0;

	/* looking for change in ValueOne*/
	channel_register_module(&testModuleCheckCall, &mod_parameter_one, 2,
	CHANNEL_NO_TIMEPRESCALE, 4);

	CU_ASSERT_EQUAL(0, mod_parameter_one.call_count);

	channel_set_value_f(testChannelOne, fVal);
	channel_step(1.0f);

	CU_ASSERT_EQUAL(0, mod_parameter_one.call_count);

	channel_set_value_f(testChannelOne, fVal);
	channel_step(1.0f);

	CU_ASSERT_EQUAL(0, mod_parameter_one.call_count);

	channel_set_value_f(testChannelOne, fVal);
	channel_step(1.0f);

	CU_ASSERT_EQUAL(0, mod_parameter_one.call_count);

	channel_set_value_f(testChannelOne, fVal);
	channel_step(1.0f);

	CU_ASSERT_EQUAL(1, mod_parameter_one.call_count);

	channel_set_value_f(testChannelOne, fVal);
	channel_step(1.0f);

	CU_ASSERT_EQUAL(1, mod_parameter_one.call_count);

	channel_set_value_f(testChannelOne, fVal);
	channel_step(1.0f);

	CU_ASSERT_EQUAL(1, mod_parameter_one.call_count);

	channel_set_value_f(testChannelOne, fVal);
	channel_step(1.0f);

	CU_ASSERT_EQUAL(1, mod_parameter_one.call_count);

	channel_set_value_f(testChannelOne, fVal);
	channel_step(1.0f);

	CU_ASSERT_EQUAL(2, mod_parameter_one.call_count);

	channel_release();
}

/**
 * Checks whether the pre-scaling on time-basis works
 */
void testChannelChangeValueTimePrescale(void) {
	ChannelId testChannelOne = 1;
	float fVal = 30.0f;

	struct ModuleCheckCallParameter mod_parameter_one;

	channel_initialize();
	channel_add(testChannelOne, ChannelDataType_Float);

	mod_parameter_one.call_count = 0;

	/* looking for change in ValueOne*/
	channel_register_module(&testModuleCheckCall, &mod_parameter_one, 2, 10.0f,
	CHANNEL_NO_EXECUTIONPRESCALE);

	CU_ASSERT_EQUAL(0, mod_parameter_one.call_count);

	channel_set_value_f(testChannelOne, fVal);
	channel_step(5.0f);

	CU_ASSERT_EQUAL(0, mod_parameter_one.call_count);

	channel_set_value_f(testChannelOne, fVal);
	channel_step(4.0f);

	CU_ASSERT_EQUAL(0, mod_parameter_one.call_count);

	channel_set_value_f(testChannelOne, fVal);
	channel_step(2.0f);

	CU_ASSERT_EQUAL(1, mod_parameter_one.call_count);

	channel_set_value_f(testChannelOne, fVal);
	channel_step(1.0f);

	CU_ASSERT_EQUAL(1, mod_parameter_one.call_count);

	channel_release();
}

/**
 * Checks that modules which registered on changing time are executed properly
 */
void testChannelChangedTime(void) {
	struct ModuleCheckCallParameter mod_parameter_one;

	channel_initialize();
	mod_parameter_one.call_count = 0;
	channel_register_module(&testModuleCheckCall, &mod_parameter_one, 1,
	CHANNEL_NO_TIMEPRESCALE,
	CHANNEL_NO_EXECUTIONPRESCALE);
	channel_step(1.0f);

	CU_ASSERT_EQUAL(1, mod_parameter_one.call_count);

	channel_release();
}

int main() {
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* add the tests to the suite */
	/* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
	if ((NULL
			== CU_add_test(pSuite,
					"test of setting of float value of a channel",
					testChannelsSetGetValueF))
			|| (NULL
					== CU_add_test(pSuite, "test of chaining changes",
							testChannelChangeValueExecuteModuleChained))
			|| (NULL
					== CU_add_test(pSuite,
							"test of changes in the time channel",
							testChannelChangedTime))
			|| (NULL
					== CU_add_test(pSuite,
							"test for appropriate callback to module",
							testChannelChangeValueExecuteModule))
			|| (NULL
					== CU_add_test(pSuite, "test for time prescales",
							testChannelChangeValueTimePrescale))
			|| (NULL
					== CU_add_test(pSuite, "test for execution prescales",
							testChannelChangeValueExecutionPrescale))
			|| (NULL
					== CU_add_test(pSuite,
							"test for smoother using rectangular method",
							test_signalp_smoother_rectangle))
			|| (NULL
					== CU_add_test(pSuite,
							"test for smoother using triangle method",
							test_signalp_smoother_triangle))
			|| (NULL
					== CU_add_test(pSuite, "test for smooth control",
							test_signalp_smooth_control))
			|| (NULL
					== CU_add_test(pSuite,
							"test for smooth control using an exponent to have of nicer ride",
							test_signalp_smooth_control_growing_exponent))
			|| (NULL
					== CU_add_test(pSuite,
							"test for smooth control growing accel and back",
							test_signalp_smooth_control_growing_slowing))) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}

