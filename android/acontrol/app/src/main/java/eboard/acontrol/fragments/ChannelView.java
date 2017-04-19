package eboard.acontrol.fragments;


import eboard.channel.Channel;
import eboard.channel.ChannelValue;

/**
 * Created by poseidon on 14.06.15.
 */
public class ChannelView {

    public Channel mDef;
    public ChannelValue mValue;

    ChannelView(Channel channelDef) {
        mDef = channelDef;
        mValue = new ChannelValue();
    }

    @Override
    public String toString() {
        return mDef.mNumber + " - " + mDef.mName + " - " + mValue.toString() + "\n" + mDef.mDescription;
    }
}
