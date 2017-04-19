package eboard.serial;

/**
 * Encapsulates a serial message send over the serial protocol
 */
public class Message {

    public MessageType m_type;

    /**
     * The message code. An integer number which can be used for easy signalling without
     * using the message payload
     */
    public int m_code;

    /**
     * Variable message playload
     */
    public MessagePayload m_payload;

    /**
     * Create an empty message without initialzing any of the required fields
     */
    public Message() {

    }

    /**
     * Create a message and specify the message type, code and payload
     */
    public Message(MessageType type, int code, MessagePayload msgPayload) {
        m_type = type;
        m_code = code;
        m_payload = msgPayload;
    }

    /**
     * Possible message types supported by the transfer protocol
     */
    public enum MessageType {
        _PLACEHOLDER_DONT_USE,
        NOP, // = 1
        INPUT,
        OUTPUT,
        // enable or disable channel streaming over serial interface
        STREAM,
        STATUS,
        HEARTBEAT,
        // to trigger unit tests
        TEST
    }

    /**
     * Possible payloads contained in the message
     */
    public enum MessagePayloadType {
        _PLACEHOLDER_DONT_USE,
        INT, // = 1
        FLOAT,
        NONE
    }

}

