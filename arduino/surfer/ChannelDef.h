/*auto generated, do no edit !*/ 

#ifndef CHANNELDEF_H
#define CHANNELDEF_H
#include "StreamChannel.h"
enum eBoardChannels{ eBoardChannel_raw_adc_1 = 1,
eBoardChannel_smoothed_adc_1 = 2,
eBoardChannel_accel_exponent = 3,
eBoardChannel_accel_min = 4,
eBoardChannel_accel_max = 5,
eBoardChannel_speed_max = 6,
eBoardChannel_motor_1_speed_set = 7,
eBoardChannel_motor_1_speed_actual = 8,
eBoardChannel_bat1_s1 = 9,
eBoardChannel_bat1_s2 = 10,
eBoardChannel_bat1_s1s2 = 11
 
 }; 

 enum eBoardChannelsMasks{ eBoardChannel_raw_adc_1_mask = 2,
eBoardChannel_smoothed_adc_1_mask = 4,
eBoardChannel_accel_exponent_mask = 8,
eBoardChannel_accel_min_mask = 16,
eBoardChannel_accel_max_mask = 32,
eBoardChannel_speed_max_mask = 64,
eBoardChannel_motor_1_speed_set_mask = 128,
eBoardChannel_motor_1_speed_actual_mask = 256,
eBoardChannel_bat1_s1_mask = 512,
eBoardChannel_bat1_s2_mask = 1024,
eBoardChannel_bat1_s1s2_mask = 2048
 
 }; 

 void eboard_init_channels() {
 channel_add( eBoardChannel_raw_adc_1, ChannelDataType_Float );
 channel_add( eBoardChannel_smoothed_adc_1, ChannelDataType_Float );
 channel_add( eBoardChannel_accel_exponent, ChannelDataType_Float );
 channel_add( eBoardChannel_accel_min, ChannelDataType_Float );
 channel_add( eBoardChannel_accel_max, ChannelDataType_Float );
 channel_add( eBoardChannel_speed_max, ChannelDataType_Float );
 channel_add( eBoardChannel_motor_1_speed_set, ChannelDataType_Float );
 channel_add( eBoardChannel_motor_1_speed_actual, ChannelDataType_Float );
 channel_add( eBoardChannel_bat1_s1, ChannelDataType_Float );
 channel_add( eBoardChannel_bat1_s2, ChannelDataType_Float );
 channel_add( eBoardChannel_bat1_s1s2, ChannelDataType_Float );
}
void eboard_stream_channels() {
 stream_channel( eBoardChannel_raw_adc_1, 1) ;
 stream_channel( eBoardChannel_smoothed_adc_1, 1) ;
 stream_channel( eBoardChannel_accel_exponent, 1) ;
 stream_channel( eBoardChannel_accel_min, 1) ;
 stream_channel( eBoardChannel_accel_max, 1) ;
 stream_channel( eBoardChannel_speed_max, 1) ;
 stream_channel( eBoardChannel_motor_1_speed_set, 1) ;
 stream_channel( eBoardChannel_motor_1_speed_actual, 1) ;
 stream_channel( eBoardChannel_bat1_s1, 1) ;
 stream_channel( eBoardChannel_bat1_s2, 1) ;
 stream_channel( eBoardChannel_bat1_s1s2, 1) ;
}
#endif
