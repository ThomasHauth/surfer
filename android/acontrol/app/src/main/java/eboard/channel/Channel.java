package eboard.channel;

/**
 * Class to define one channel
 */
public class Channel {

    /**
     * Channel number. Used to identify the channel on the serial communication protocoll.
     */
    public int mNumber;

    /**
     * String name of the channel.
     */
    public String mName;

    /**
     * String description of the channel.
     */
    public String mDescription;

    /**
     * Data type used by this channel
     */
    public DataType mDataType;

    /**
     * Create a new channel
     */
    Channel(int number, String name, String desc, DataType dt) {
        mNumber = number;
        mName = name;
        mDescription = desc;
        mDataType = dt;
    }

    /**
     * Possible data types of channels
     */
    public enum DataType {
        Float,
        Int
    }
}
