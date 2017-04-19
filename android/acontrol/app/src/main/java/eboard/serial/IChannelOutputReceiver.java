package eboard.serial;


/**
 * Interface which can be implemented by classes which want to subscribe to for
 * channel change events.
 */
public interface IChannelOutputReceiver {
    void change(int channel, MessagePayload payload);
}
