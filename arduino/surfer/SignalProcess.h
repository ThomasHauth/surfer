#ifndef SIGNALPROCESS
#define SIGNALPROCESS

#include "Channels.h"

#define SIGNALP_MAX_WINDOW_SIZE 32

/* workaround for c-based functions according to 
http://stackoverflow.com/questions/19635007/arduino-c-undefined-reference-to-readarms */
#ifdef __cplusplus
extern "C" {
#endif


char equal_float ( float f1, float f2, float maxDelta );
float max_float ( float f1, float f2 );
float min_float ( float f1, float f2 );

enum SignalSmoothingAlgorithm {
  SignalSmoothingAlgorithm_Rectangular = 1,
  SignalSmoothingAlgorithm_Triangle
};

struct signalp_smoother_parameter {
  ChannelId input_channel_id;
  ChannelId output_channel_id;
  
  int window_size;
  enum SignalSmoothingAlgorithm algorithm;

  /* internal */
  float buffer[SIGNALP_MAX_WINDOW_SIZE];
  int buffer_position;
};


void signalp_smoother_initialize( struct signalp_smoother_parameter * );
int signalp_smoother( void * );

// res = a - b
struct signalp_substract_parameter{ 
  ChannelId input_channel_a_id;
  ChannelId input_channel_b_id;
  ChannelId output_channel_id;
};

// only works for float channels atm !
int signalp_substract( void *);


struct signalp_smooth_control_parameter {
  ChannelId set_channel_id;
  ChannelId actual_channel_id;

  // growth is modelling how fast the 
  // actual value will approach the set 
  // value

  // the initial growth value if set is larger
  // than actual
  // all in seconds
  ChannelId accel_max_channel_id;
  // the maximum
  ChannelId accel_min_channel_id;
  
  ChannelId accel_exponent_channel_id;
  
  // the maximum speed which will be set on the
  // actual channel. All larger values will be ignored
  ChannelId speed_max_channel_id;
  
  
  // the time span which is between calls to the control function
  // in seconds
  float control_time;
};

int signalp_smooth_control( void *);

#ifdef __cplusplus
}
#endif

#endif
