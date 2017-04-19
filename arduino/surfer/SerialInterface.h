
#ifndef SERIALINTERFACE_H
#define SERIALINTERFACE_H

/* workaround for c-based functions according to 
needed here, because functions might be called from c functions
http://stackoverflow.com/questions/19635007/arduino-c-undefined-reference-to-readarms */
#ifdef __cplusplus
extern "C" {
#endif

/*

command examples:

set the int value of channel 3 to 255
%2%3%1%255%

*/


enum SerialMessageType     {
  SerialMessageType_NOP = 1,
  SerialMessageType_INPUT,
  SerialMessageType_OUTPUT,
  // enable or disable channel streaming over serial interface
  SerialMessageType_STREAM,
  SerialMessageType_STATUS,
  SerialMessageType_HEARTBEAT,
  // to trigger unit tests
  SerialMessageType_TEST
};

enum SerialMessagePayload  {
  SerialMessagePayload_Int = 1,
  SerialMessagePayload_Float,
  SerialMessagePayload_None
};


struct SerialMessage {

  enum SerialMessageType m_type;
  char m_code;
  
  enum SerialMessagePayload m_payloadType;
  //String m_payloadString;
  int m_payloadInt;
  float m_payloadFloat;
  
  struct SerialMessage * m_next;
};

void ser_initialize();

struct SerialMessage * ser_retrieve();

void ser_transmit( struct SerialMessage * firstMessage );

struct SerialMessage * ser_allocate( enum SerialMessageType, char );

void ser_free_messages( struct SerialMessage *   );

void ser_heartbeat();


#ifdef EMULATE_SERIAL
void ser_initialize(){}

struct SerialMessage * ser_retrieve() { return 0;}

void ser_transmit( struct SerialMessage * firstMessage ) {}

struct SerialMessage * ser_allocate( enum SerialMessageType t, char ch ) {return 0;}

void ser_free_messages( struct SerialMessage * msg  ){}

void ser_heartbeat(){}
#endif


#ifdef __cplusplus
}
#endif

#endif
