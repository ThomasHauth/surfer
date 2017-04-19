#include <Servo.h>

#include "SerialInterface.h"

#include "Channels.h"
#include "StreamChannel.h"
#include "SignalProcess.h"
#include "ADCReadout.h"
#include "PWMWrite.h"
#include "Logging.h"
#include "MotorControl.h"
#include "ChannelDef.h"

unsigned long last_time;
unsigned long delta_time;

void setup() {
  
  // analog input pins
  const int pin_battery1_s1 = 8;
  const int pin_battery1_s1s2 = 10;
  
  // put your setup code here, to run once:
  last_time = micros();
  delta_time = 0;
  
  // initialize serial communication at 9600 bits per second:
  //Serial.begin(9600);
  
  ser_initialize();
  
  SerialMessage * msg = ser_allocate(SerialMessageType_STATUS, 55);
  
  ser_transmit(msg); 
  ser_free_messages( msg );
  
  // setup channels
  channel_initialize();
  
  eboard_init_channels();
  
  // just for debugging, remove later
  // start streaming two channels
  eboard_stream_channels();
  
  // ### READ Battery 1 Cell 1, Arduino due ref is 3.3 V !!!
  adcreadout_parameter * bat1_s1_parameter = new adcreadout_parameter(); 
  bat1_s1_parameter->pin = pin_battery1_s1;
  bat1_s1_parameter->write_channel_id = eBoardChannel_bat1_s1;
 
  adcreadout_initialize(bat1_s1_parameter);
  // a bit nasty this has to be done here ...
  bat1_s1_parameter->float_transform = 1;
  bat1_s1_parameter->float_min = 0.0;
  bat1_s1_parameter->float_max = 3.3 * 2.0; // v with factor 2 splitter
  
  channel_register_module( adcreadout_execute, bat1_s1_parameter, 1,
    1000000,
    CHANNEL_NO_EXECUTIONPRESCALE );

  // ### READ Battery 1 Cell 1+2
  adcreadout_parameter * bat1_s1s2_parameter = new adcreadout_parameter(); 
  bat1_s1s2_parameter->pin = pin_battery1_s1s2;
  bat1_s1s2_parameter->write_channel_id = eBoardChannel_bat1_s1s2;
 
  adcreadout_initialize(bat1_s1s2_parameter);
  // a bit nasty this has to be done here ...
  bat1_s1s2_parameter->float_transform = 1;
  bat1_s1s2_parameter->float_min = 0.0;
  bat1_s1s2_parameter->float_max = 3.3 * 3.0; // v, with factor 3 splitter
  
  channel_register_module( adcreadout_execute, bat1_s1s2_parameter, 1,
    1000000,
    CHANNEL_NO_EXECUTIONPRESCALE );
  
  // ### Compute cell 2 voltage
  signalp_substract_parameter * comp_s2 = new signalp_substract_parameter;
  comp_s2-> input_channel_a_id = eBoardChannel_bat1_s1s2;
  comp_s2-> input_channel_b_id = eBoardChannel_bat1_s1;
  comp_s2-> output_channel_id = eBoardChannel_bat1_s2;
  
    channel_register_module( signalp_substract, comp_s2, eBoardChannel_bat1_s1_mask | eBoardChannel_bat1_s1s2_mask,
    CHANNEL_NO_TIMEPRESCALE,
    CHANNEL_NO_EXECUTIONPRESCALE );

  // ### Motor 1 speed Actual - only set by smooth control
   struct motor_set_parameter * motor1_speed_actual = new motor_set_parameter();
   motor1_speed_actual->pin = 2;
   motor1_speed_actual->read_channel_id = eBoardChannel_motor_1_speed_actual;
  
   channel_set_value_f(eBoardChannel_motor_1_speed_set, 0.0f);
   channel_set_value_f(eBoardChannel_motor_1_speed_actual, 0.0f);
  
   motor_set_initialize ( motor1_speed_actual ); 
  
   channel_register_module( motor_set_execute, motor1_speed_actual, eBoardChannel_motor_1_speed_actual_mask,
    CHANNEL_NO_TIMEPRESCALE,
    CHANNEL_NO_EXECUTIONPRESCALE );
   
   // set default values for motor acceleration / deacceleration 
   // per second
   channel_set_value_f(eBoardChannel_accel_min, 0.07);
   channel_set_value_f(eBoardChannel_accel_max, 0.17);
   channel_set_value_f(eBoardChannel_speed_max, 0.5);
   channel_set_value_f(eBoardChannel_accel_exponent, 1.4f);
    
   // ### Motor 1 Speed Set 
   struct signalp_smooth_control_parameter * motor1_speed_set = new signalp_smooth_control_parameter();
   motor1_speed_set->set_channel_id = eBoardChannel_motor_1_speed_set;
   motor1_speed_set->actual_channel_id = eBoardChannel_motor_1_speed_actual;

   motor1_speed_set->accel_min_channel_id = eBoardChannel_accel_min;
   motor1_speed_set->accel_max_channel_id = eBoardChannel_accel_max;
   motor1_speed_set->speed_max_channel_id = eBoardChannel_speed_max;
   motor1_speed_set->accel_exponent_channel_id = eBoardChannel_accel_exponent;
   motor1_speed_set->control_time = 0.05f;

   channel_register_module(  signalp_smooth_control, motor1_speed_set, 1,
         // convert to usceconds
        (int) motor1_speed_set->control_time * 1000 * 1000,
        CHANNEL_NO_EXECUTIONPRESCALE );
      
    // stream all channels every 2 seconds
    // first tests show a 25 us control loop which translates to 40 kHz
    stream_transmit_parameter * stream_dt = new stream_transmit_parameter();
    stream_dt->channel_id = 0;
      
    channel_register_module( stream_transmit_channel, stream_dt, 1,
        2000000,
        CHANNEL_NO_EXECUTIONPRESCALE );
}



void loop() {

  // read possible serial input
  SerialMessage * first_message = ser_retrieve();
  SerialMessage * next_message = first_message;
  //printf("loop \n");
  while (next_message != 0) {
    //printf("got message \n");
    // to start streaming a channel, send
    // %4%0%1%100%
    // to set a value on channel 1
    // %2%1%2%100%
    
    if (next_message->m_type == SerialMessageType_STREAM) {
     // update streaming settings for some channel
     stream_channel( next_message->m_code, next_message->m_payloadInt); 
    
    } else if (next_message->m_type == SerialMessageType_INPUT) {
      if ( next_message->m_payloadType == SerialMessagePayload_Float ) {
        LOG_DEBUG("Input message for channel %i with float value %f received\n", (ChannelId) next_message->m_code, next_message->m_payloadFloat );
       channel_set_value_f( (ChannelId) next_message->m_code, next_message->m_payloadFloat);
      } else if ( next_message->m_payloadType == SerialMessagePayload_Int ) {
        LOG_DEBUG("Input message for channel %i with int value %i received\n", (ChannelId) next_message->m_code, next_message->m_payloadInt );
        channel_set_value_i( (ChannelId) next_message->m_code, next_message->m_payloadInt);
      } else { 
        LOG_ERROR("Cannot process input type %i\n", next_message->m_payloadType); 
      }
    } else if (next_message->m_type == SerialMessageType_HEARTBEAT ) {
       ser_heartbeat();
    }   
    else {
       LOG_ERROR("Cannot process serial message of type %i\n", next_message->m_type); 
    }
    
    next_message = next_message->m_next;
  }
  
  ser_free_messages(first_message);
  // feed channel system ..
  

  // stream channels
  stream_subscribed_channels( delta_time );

  // time wrap-around not supported
  // micros will wrap around after 77 hours
  long this_time = micros();
  delta_time = this_time - last_time;
  channel_step(delta_time);
  last_time = this_time;
}

