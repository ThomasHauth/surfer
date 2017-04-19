package eboard.channel;

import java.util.HashMap;

import eboard.serial.IChannelOutputReceiver;
import eboard.serial.MessagePayload;

/**
 * This class buffers all the channel values which have been transferred
 * via bluetooth so they can be easily retrieved whenever required.
 */
public class ChannelBuffer implements IChannelOutputReceiver {

    /**
     * Update the values stored for a channel
     */
    @Override
    public void change(int channel, MessagePayload payload) {
        mBuffer.put(channel,payload);
    }

    /**
     * Return the buffered value for a channel
     */
    public MessagePayload getBuffered( int channel)
    {
        return mBuffer.get(channel);
    }

    HashMap<Integer, MessagePayload> mBuffer = new HashMap<>();
}
