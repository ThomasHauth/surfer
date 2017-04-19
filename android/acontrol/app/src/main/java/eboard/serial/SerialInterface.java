package eboard.serial;

import android.util.Log;

import java.util.ArrayList;
import java.util.List;

/**
 * This class parses the serial input stream into the Message classes and
 * can take a message class and generate the serial output stream.
 */
public class SerialInterface {

    private String LogTag = "AControlSerialInterface";

    /**
     * Return the output string which can be send over the serial connection
     * from an input message.
     */
    public String generateOutput(Message msg) {
        String output = "%";

        output = output + msg.m_type.ordinal() + "%";
        output = output + msg.m_code + "%";
        output = output + msg.m_payload.m_type.ordinal() + "%";

        if (msg.m_payload.m_type == Message.MessagePayloadType.FLOAT) {
            output = output + msg.m_payload.m_float + "%";
        } else if (msg.m_payload.m_type == Message.MessagePayloadType.INT) {
            output = output + msg.m_payload.m_int + "%";
        } else if (msg.m_payload.m_type == Message.MessagePayloadType.NONE) {
            output = output + "%";
        } else {
            Log.e(LogTag, "Payload type " + msg.m_payload.m_type + " not supported.");
            return null;
        }

        return output;
    }

    /**
     * Parse an input string into one (or more) Messages.
     */
    public List<Message> parseInput(String sInput) {

        ArrayList<Message> msgs = new ArrayList<>();

        String[] splitted = sInput.split("%");
        // first value will be ""

        if (splitted.length < 5) {
            Log.e(LogTag, "Discarded serial message because to few parameters: " + sInput);
            return msgs;
        }

        Message msg = new Message();
        int iMsgType = Integer.parseInt(splitted[1]);

        if ((iMsgType < 0) && (iMsgType >= Message.MessageType.values().length)) {
            Log.e(LogTag, "Message type " + iMsgType + " not supported");
            return msgs;
        }
        msg.m_type = Message.MessageType.values()[iMsgType];
        msg.m_code = Integer.parseInt(splitted[2]);

        int iPayloadType = Integer.parseInt(splitted[3]);

        if ((iPayloadType < 0) && (iPayloadType >= Message.MessagePayloadType.values().length)) {
            Log.e(LogTag, "Message payload type " + iPayloadType + " not supported");
            return msgs;
        }
        Message.MessagePayloadType payloatType = Message.MessagePayloadType.values()[iPayloadType];

        if (payloatType == Message.MessagePayloadType.FLOAT) {
            msg.m_payload = new MessagePayload(Float.parseFloat(splitted[4]));
        } else if (payloatType == Message.MessagePayloadType.INT) {
            msg.m_payload = new MessagePayload(Integer.parseInt(splitted[4]));
        } else if (payloatType == Message.MessagePayloadType.NONE) {
            msg.m_payload = new MessagePayload();
        } else {
            Log.e(LogTag, "Message payload type " + iPayloadType + " not supported");
            return msgs;
        }

        msgs.add(msg);

        return msgs;
    }

}
