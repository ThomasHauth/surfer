package eboard.channel;

/**
 * Container for the various values a channel can hold. Only one of the values (float,int) is
 * filled at one time
 */
public class ChannelValue {
    public Float mFloat = null;
    public Integer mInt = null;

    public void setFloatValue(float f) {
        mInt = null;
        mFloat = f;
    }

    public void setIntValue(int i) {
        mFloat = null;
        mInt = i;
    }

    @Override
    public String toString() {
        if (mFloat != null) {
            return mFloat.toString();
        }

        if (mInt != null) {
            return mInt.toString();
        }

        return "undef";
    }
}