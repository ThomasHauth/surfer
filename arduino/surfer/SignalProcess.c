#include "SignalProcess.h"

#include "Logging.h"

#include <math.h>

char equal_float ( float f1, float f2, float maxDelta ) {
    return fabs( f1 - f2 ) < maxDelta;
}

float max_float ( float f1, float f2 ) {
    return f1 > f2 ? f1 : f2;
}

float min_float ( float f1, float f2 ) {
    return f1 < f2 ? f1 : f2;
}


int signalp_smoother( void * parameter ) {
  struct signalp_smoother_parameter * s_parameter = (struct signalp_smoother_parameter *)parameter;
  float current_v, smoothed_v, sum;
  int i;
  int previous_i;
  unsigned int trig_divider;

  /* update the buffer with newest value */
  current_v = channel_get_value_f(s_parameter->input_channel_id);

    /* check if this is the first execution and fill the buffer */
    if (s_parameter->buffer_position < 0) {
        for ( i = 0; i < s_parameter->window_size; i++ ) {
            s_parameter->buffer[ i ] = current_v;
        }
        s_parameter->buffer_position = 0;
    }

    s_parameter->buffer[ s_parameter->buffer_position ] = current_v;
    s_parameter->buffer_position ++;
    /* wrap around */
    if ( s_parameter->buffer_position == s_parameter->window_size )
        s_parameter->buffer_position = 0;

  /* apply the algorithm */
  if (s_parameter->algorithm == SignalSmoothingAlgorithm_Rectangular ) {
    sum = 0;

    for ( i = 0; i < s_parameter->window_size; i++ ) {
        sum += s_parameter->buffer[ i ];
    }
    smoothed_v = sum / s_parameter->window_size;

    LOG_DEBUG("AVG Sum %f | Current v %f | smoothed v %f", sum, current_v, smoothed_v );
  } 
  else if (s_parameter->algorithm == SignalSmoothingAlgorithm_Triangle ) {
    sum = 0;
    trig_divider = 0;

    /* points the previous item (going backwards in buffer) */ 
    previous_i = s_parameter->buffer_position - 1;
    for ( i = s_parameter->window_size; i > 0 ; i-- ) {
        /* wrap around */
        if (previous_i < 0) {
            previous_i = s_parameter->window_size - 1;
        }
        sum += s_parameter->buffer[ previous_i ] * i;
        trig_divider += i;
        previous_i = previous_i - 1;
    }

    smoothed_v = ( sum / trig_divider );

    LOG_DEBUG("AVG Trig Sum %f | Current v %f | smoothed v %f", sum, current_v, smoothed_v );
  } else {
    LOG_ERROR("Smoothing algorithm %i not implemented", s_parameter->algorithm);
    smoothed_v = current_v;
  }
    
    channel_set_value_f(s_parameter->output_channel_id, smoothed_v);


    return 0;
}

void signalp_smoother_initialize( struct signalp_smoother_parameter * parameter ) {
    parameter->buffer_position = -1;
}

int signalp_substract( void * p) {
   struct signalp_substract_parameter * casted_p = ( struct signalp_substract_parameter *) p;
   
   float a = channel_get_value_f(casted_p->input_channel_a_id);
   float b = channel_get_value_f(casted_p->input_channel_b_id);
   
   channel_set_value_f(casted_p->output_channel_id, a - b);

   return 0;
}


int signalp_smooth_control( void * _p) {
  struct signalp_smooth_control_parameter * p = (struct signalp_smooth_control_parameter *) _p;

  const float actual = channel_get_value_f(p->actual_channel_id);
  const float set = channel_get_value_f(p->set_channel_id);

  // compute control distance
  const float dist = set - actual;

  // don't do anything if we are already very close to the set value
  if ( equal_float( dist, 0, 0.005f )) 
    return 0;

  const float growth_min = channel_get_value_f(p->accel_min_channel_id);
  const float growth_exponent = channel_get_value_f(p->accel_exponent_channel_id);
  const float growth_max = channel_get_value_f(p->accel_max_channel_id);

  // compute maximum change at this level of actual value
  const float max_change_dt = (growth_max - growth_min) * pow(actual,growth_exponent) + growth_min;

  float max_change_timespan = max_change_dt * p->control_time;
  float this_change = 0.0f;

  // also allow slowing down
  if ( dist < 0.0f ) {
    max_change_timespan = -max_change_timespan;
    this_change = max_float(max_change_timespan, dist );
  } else {
    this_change = min_float(max_change_timespan, dist );
  }

  const float speed_max = channel_get_value_f(p->speed_max_channel_id);

  // set the new actual value while respecting the maximum speed
  channel_set_value_f(p->actual_channel_id, min_float(actual + this_change, speed_max));
  return 0;
}


