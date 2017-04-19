package eboard.serial;

/**
 * This class encapsulates the possible message payloads
 * which can be attached to a message.
 */
public class MessagePayload {

    public Message.MessagePayloadType m_type;
    public float m_float;
    public int m_int;

    /**
     * Create a message payload which holds a float
     */
    public MessagePayload(float f) {
        m_float = f;
        m_type = Message.MessagePayloadType.FLOAT;
    }

    /**
     * Create a message payload which holds an integer
     */
    public MessagePayload(int i) {
        m_int = i;
        m_type = Message.MessagePayloadType.INT;
    }

    public String stringValue() {
        if (m_type == Message.MessagePayloadType.INT) {
            return new Integer(m_int).toString();
        }
        else if (m_type == Message.MessagePayloadType.FLOAT) {
            return new Float(m_float).toString();
        }
        return "type not supported";
    }

    /**
     * Create this class with no payload contained
     */
    public MessagePayload() {
        m_type = Message.MessagePayloadType.NONE;
    }
}
