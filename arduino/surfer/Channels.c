#include "Channels.h"
#include "Logging.h"

    union ChannelValueUnion{
        float f;
        int i;
        unsigned long ul;
    } ;

struct ChannelConfig {
    enum ChannelDataType type;
    char changed;
    union ChannelValueUnion value;
};

struct ChannelModuleConfig {
    char excecuted;
    ChannelModuleExecute execute;
    ChannelMask mask;
    void * parameter;
    int execution_prescale;
    int skipped_executions;
    ChannelTime time_prescale;
    ChannelTime time_to_wait;
};

struct ChannelConfig g_channel [ CHANNEL_MAX_CHANNELS ];

struct ChannelModuleConfig g_modules [ CHANNEL_MAX_MODULES ];
unsigned int g_moduleCount;


void channel_initialize() {

    unsigned int i = 0;

    // set all channels to unused
    for (; i < CHANNEL_MAX_CHANNELS; i++ ) {
        g_channel[i].type = ChannelDataType_Unused;
    }

    // add the primary timing channel
    channel_add( 0, ChannelDataType_ULong );
    // no modules at first
    g_moduleCount = 0;
}

void channel_add( ChannelId chanId, enum ChannelDataType dt ) {
    LOG_DEBUG( "Adding Channel with id %i with type %i\n", chanId, dt ) 

    g_channel [ chanId ].type = dt;
    g_channel [ chanId ].changed = 0;

    switch ( dt ) {
    case ChannelDataType_Unused:
        LOG_ERROR ( "cannot set value on unused channel %u", chanId );
        break;
    case ChannelDataType_Float:
        g_channel [ chanId ].value.f = 0.0f;
        break;
    case ChannelDataType_Int:
        g_channel [ chanId ].value.i = 0;
        break;
    case ChannelDataType_ULong:
        g_channel [ chanId ].value.ul = 0;
        break;
    }
}

void channel_register_module( ChannelModuleExecute execute, void * parameter,
     ChannelMask mask, ChannelTime time_prescale, int execution_prescale) {
    LOG_DEBUG( "Registering ChannelModule number %i\n", g_moduleCount ) 

    if ( ( time_prescale > 0 ) && ( execution_prescale > 0 )) {
        LOG_ERROR("Execution and time prescale can not be combined");
    }

    g_modules[g_moduleCount].execute = execute;
    g_modules[g_moduleCount].mask = mask;
    g_modules[g_moduleCount].parameter = parameter;
    g_modules[g_moduleCount].excecuted = 0;
    g_modules[g_moduleCount].execution_prescale = execution_prescale;
    g_modules[g_moduleCount].skipped_executions = 0;
    g_modules[g_moduleCount].time_prescale = time_prescale;
    g_modules[g_moduleCount].time_to_wait = time_prescale;

    g_moduleCount++;
}

void _internal_channel_reset_changed() {
    unsigned int i = 0;
    for (; i < CHANNEL_MAX_CHANNELS; i++ ) {
        g_channel[i].changed = 0;
    }
}

void channel_set_value_f( ChannelId id, float f) {
    g_channel[id].value.f = f;
    g_channel[id].changed = 1;
}

float channel_get_value_f( ChannelId id ) {
    return g_channel[id].value.f;
}

void channel_set_value_ul( ChannelId id, unsigned long ul) {
    g_channel[id].value.ul = ul;
    g_channel[id].changed = 1;
}

unsigned long channel_get_value_ul( ChannelId id ) {
    return g_channel[id].value.ul;
}

void channel_set_value_i( ChannelId id, int i) {
    g_channel[id].value.i = i;
    g_channel[id].changed = 1;
}
  
int channel_get_value_i( ChannelId id ) {
    return g_channel[id].value.i;  
}

enum ChannelDataType channel_get_data_type( ChannelId id ) {
    return g_channel[id].type;
}


void channel_step( ChannelTime dt ) {
    ChannelMask changedChannels = 0;
    char done = 0;
    char execute = 0;

    /* update special time channel 0 */
    channel_set_value_ul(0, dt);
   
    while ( done < 125 ) {
        // check which channels were actually changed ...
        changedChannels = 0;
        // reverse to have the low ids on the right hand side of the bitmask
        int i = CHANNEL_MAX_CHANNELS - 1;
        for (; i >= 0; i-- ) {
            changedChannels = changedChannels << 1;
            if (g_channel[i].type != ChannelDataType_Unused) {
                changedChannels = changedChannels | (g_channel[i].changed == 1);
            }
        }
        if (changedChannels == 0) {
            //LOG_DEBUG("No channel changes, exiting\n")
            break;
        }
        //LOG_DEBUG("change mask is %lu\n", changedChannels)
        _internal_channel_reset_changed();

        for ( i = 0; i < g_moduleCount; i++ ) {
            execute = 1;
            // check if the bitmask is positive
            execute &= ( changedChannels & g_modules[i].mask ) > 0;
                // todo: check the output
            if (g_modules[i].execution_prescale > 0 ) {
                g_modules[i].skipped_executions++;
                if (g_modules[i].skipped_executions == g_modules[i].execution_prescale ) {
                    /* excute wil be still one, so just don't touch it*/
                    g_modules[i].skipped_executions = 0;
                } else {
                    execute = 0;
                }
            }

            if (g_modules[i].time_prescale > 0 ) {
                g_modules[i].time_to_wait -= dt;
                if (g_modules[i].time_to_wait < 0){
                    g_modules[i].time_to_wait = g_modules[i].time_prescale;
                } else {
                    execute = 0;
                }
            }

            if (execute > 0) {
                LOG_DEBUG("Executing module %u\n", i);
                g_modules[i].execute( g_modules[i].parameter ); 
            }
        }
        // make sure we don't get stuck in an endless loop
        done++;
        if ( done > 120 ) {
            LOG_ERROR("Channel step loop approaching its maximum iterations. Endless relation ?\n");
        }
    }
}

void channel_release() {

}




