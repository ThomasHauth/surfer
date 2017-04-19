#include "PWMWrite.h"

#include "Logging.h"

#include <Arduino.h>

int pwmwrite_execute( void * parameter) {
    struct pwmwrite_parameter * casted_parameter = ( struct pwmwrite_parameter*)parameter;

    // check which channel formal we have to read
    enum ChannelDataType ct = channel_get_data_type( casted_parameter->read_channel_id );
    switch (ct) {
    case ChannelDataType_Int:
        int iv = channel_get_value_i(casted_parameter->read_channel_id);
        LOG_ERROR("writing to pin %i\n", iv);
        analogWrite(casted_parameter->pin, iv );
        break;
   /* default:
        LOG_ERROR("Data type %i not supportd by pwm write\n", ct);*/
    }
    
    return 0;
}
