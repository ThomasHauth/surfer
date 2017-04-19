package eboard.serial;

import android.util.Log;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;

/**
 * This class parses serial messages from Bluetooth via the recvMessage() method
 * and implements a subscriber patterns to distributes the parsed message's content
 * to all subscribed components.
 */
public enum BTSerial {
    INSTANCE, BTSerial;

    OutputStream mOutputStream;
    List<ISerialMessageReceiver> m_recvs = new ArrayList<ISerialMessageReceiver>();
    List<IChannelOutputReceiver> m_channelOutput = new ArrayList<IChannelOutputReceiver>();

    /**
     * Parse a incoming message and send to all subscribed receivers
     */
    public void recvMessage(String message) {
        for (ISerialMessageReceiver r : m_recvs) {
            r.recv(message);
        }

        // parse Message
        SerialInterface si = new SerialInterface();
        List<Message> msgs = si.parseInput(message);

        for (Message msg : msgs) {
            // notify about incoming message of certain types
            if (msg.m_type == Message.MessageType.OUTPUT) {
                for (IChannelOutputReceiver r : m_channelOutput) {
                    r.change(msg.m_code, msg.m_payload);
                }
            }
        }
    }

    /**
     * Subscribe a new message receiver which gets notified when a new
     * message has been parsed.
     */
    public void subscribeMessage(ISerialMessageReceiver recv) {

        m_recvs.add(recv);
    }

    public void subscribeChannelOutput(IChannelOutputReceiver recv) {
        m_channelOutput.add(recv);
    }

    /**
     * Send a new message to the output stream of the bluetooth connection
     */
    public void send(String message) {
        Log.i("acontrol", "Going to send: " + message);

        if (mOutputStream != null) {
            try {
                mOutputStream.write(message.getBytes());
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * Set the output stream for BT serial transmission
     */
    public void setOutputStream(OutputStream outputs) {
        mOutputStream = outputs;
    }
}
