#include "SerialInterface.h"

#include <Arduino.h>
#include "Logging.h"

// supplies arduino specific stuff, like Serial
//#include <Arduino.h>

void ser_initialize(void) {
    // tx1 and rx1 which is connected to BT
    Serial.begin(9600);
}  


int nextCharacter( String *s, int current_pos ) {
  int found_pos = -1;
  
  while ( s->length() > current_pos ) {
    if (s->charAt( current_pos ) == '%') {
      found_pos = current_pos;
      break;
    } 
    current_pos ++;
  } 
  
  return found_pos;
}

String parseNext( String *s, int current_pos, int * next_pos ){
  int start_seq = nextCharacter(s, current_pos );
  if (start_seq < 0 ) {
     start_seq = current_pos;
  } else {
     // move one forward from the delimiter character 
     start_seq ++; 
  }
  
  int end_seq = nextCharacter(s, start_seq + 1 );
  if (end_seq < 0 )
     return "";
  
  *next_pos = end_seq;   
  return s->substring(start_seq, end_seq);
}
  
  
SerialMessage * ser_parse_messages( String inString) {
    SerialMessage * thisMessage = 0;

    /* exit if not a command string */
    if (! inString.startsWith("%"))
      return 0;
        
    int next_pos = 0;
    String s_type = parseNext( &inString,next_pos, &next_pos);
    String s_code = parseNext( &inString,next_pos, &next_pos);
    String s_payload_type = parseNext( &inString,next_pos, &next_pos);
    String s_payload = parseNext( &inString,next_pos, &next_pos);

    // everything, except payload, must be provided
    if ( ( s_type.length() == 0 ) || ( s_code.length() == 0 ) || ( s_payload_type.length() == 0 ) ) {
      LOG_ERROR("Received incomplete message\n");
      return 0;
      //continue;
    }
    
    int message_type = s_type.toInt();
    int message_code = s_code.toInt();
    SerialMessagePayload message_payload_type = (SerialMessagePayload) s_payload_type.toInt();
    
    // parse
    thisMessage = ser_allocate( (enum SerialMessageType) message_type, message_code);
    thisMessage->m_payloadType = message_payload_type;
    
    // check to parse the payload
    if ( message_payload_type == SerialMessagePayload_None ){ 
      // no parsing necessary
    } else if ( message_payload_type == SerialMessagePayload_Float ) { 
      thisMessage->m_payloadFloat = s_payload.toFloat();
    } else if ( message_payload_type == SerialMessagePayload_Int ) { 
      thisMessage->m_payloadInt = s_payload.toInt();
    } else {
       LOG_ERROR("Cannot parse message payload type %i\n", (int) message_payload_type );
    }

    LOG_DEBUG("Message type %i code %i received\n", message_type, message_code);

    return thisMessage;
}

  SerialMessage * ser_retrieve() {
    SerialMessage * firstMessage = 0;
    SerialMessage * thisMessage = 0;
    SerialMessage * prevMessage = 0;
    SerialMessage * ser_message = 0;
        
    while (  Serial.available() > 0  ) {
      thisMessage = ser_parse_messages(Serial.readString());
      if ( firstMessage == 0) {
         firstMessage = thisMessage; 
      }
      if ( prevMessage != 0 ) {
        prevMessage->m_next = thisMessage;
      }
      prevMessage = thisMessage;
    }
    
    while (  Serial.available() > 0 ) {
      thisMessage = ser_parse_messages(Serial.readString());
      if ( firstMessage == 0) {
         firstMessage = thisMessage; 
      }
      if ( prevMessage != 0 ) {
        prevMessage->m_next = thisMessage;
      }
      prevMessage = thisMessage;
    }
    
    return firstMessage;
  }
  
void ser_transmit_print_i( int i) {
  Serial.print(i);
}

void ser_transmit_print_c( const char * c) {
  Serial.print(c);
}

void ser_transmit_print_f( float f) {
  Serial.print(f);
}

void ser_transmit( SerialMessage * first ) {

  SerialMessage * next = first;
  while ( next != 0 ) {
    
    LOG_DEBUG("Sending message of type %i", (int)next->m_type)  ;
    
    ser_transmit_print_c("%");
    ser_transmit_print_i((int)next->m_type);
    ser_transmit_print_c("%");
    ser_transmit_print_i((int)next->m_code);
    ser_transmit_print_c("%");
    ser_transmit_print_i((int)next->m_payloadType);
    ser_transmit_print_c("%");
    
    if ( next->m_payloadType == SerialMessagePayload_Float ) {
      ser_transmit_print_f( next->m_payloadFloat ); 
    }
    else if ( next->m_payloadType == SerialMessagePayload_Int ) {
      ser_transmit_print_i( next->m_payloadInt ); 
    }
    else if ( next->m_payloadType == SerialMessagePayload_None ) {
      // Do nothing
    }
    else {
      LOG_ERROR("Payload %i not supported by serial interface\n", next->m_payloadType ); 
    }
    
    ser_transmit_print_c("%");
    ser_transmit_print_c("\n");
    
    LOG_DEBUG("Message sent");
    
    next = next->m_next;
  }
    
}

void ser_heartbeat () 
{ 
  SerialMessage * msg = ser_allocate(SerialMessageType_HEARTBEAT, 66);
  
  ser_transmit(msg); 
  ser_free_messages( msg );
}

SerialMessage * ser_allocate( SerialMessageType type, char code ) {
  SerialMessage * msg = (SerialMessage *) malloc( sizeof(SerialMessage));
  
  msg->m_type = type;
  msg->m_code = code;
  msg->m_next = 0;
  msg->m_payloadType = SerialMessagePayload_None;
  
  return msg;
}

void ser_free_messages( SerialMessage * msg ) {
  if (msg == 0)
    return;
  
  if (msg->m_next != 0) {
    ser_free_messages(msg->m_next); 
  }
  free(msg);
}

