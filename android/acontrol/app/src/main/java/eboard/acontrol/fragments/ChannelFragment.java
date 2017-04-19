package eboard.acontrol.fragments;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.res.Configuration;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.text.InputType;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;

import java.util.ArrayList;

import eboard.acontrol.R;
import eboard.channel.Channel;
import eboard.channel.ChannelDef;
import eboard.serial.BTSerial;
import eboard.serial.IChannelOutputReceiver;
import eboard.serial.Message;
import eboard.serial.MessagePayload;
import eboard.serial.SerialInterface;

/**
 * A placeholder fragment containing a simple view.
 */
public class ChannelFragment extends Fragment implements IChannelOutputReceiver {
    ListView mChannelList = null;
    ArrayAdapter<ChannelView> mAdapter = null;

    public ChannelFragment() {
    }

    /**
     * Returns a new instance of this fragment for the given section
     * number.
     */
    public static ChannelFragment newInstance() {
        ChannelFragment fragment = new ChannelFragment();

        return fragment;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        final String LogTag = "acontrol";
        View rootView = inflater.inflate(R.layout.channels_acontrol, container, false);
        mChannelList = (ListView) rootView.findViewById(R.id.channels_list);

        ArrayList<ChannelView> channel_Names = new ArrayList<>();

        for (Channel c : ChannelDef.mChannels) {
            channel_Names.add(new ChannelView(c));
        }

        ChannelView[] values = channel_Names.toArray(new ChannelView[channel_Names.size()]);


        mAdapter = new ArrayAdapter<ChannelView>(getActivity(),
                android.R.layout.simple_list_item_1, values);
        mChannelList.setAdapter(mAdapter);

        mChannelList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                final ChannelView vItem = (ChannelView) parent.getAdapter().getItem(position);
                Toast.makeText(view.getContext(), "This is " + vItem.mDef.mName, Toast.LENGTH_SHORT).show();

                AlertDialog.Builder alert = new AlertDialog.Builder(view.getContext());
                alert.setTitle("Title");
                final EditText input = new EditText(view.getContext());
                input.setInputType(InputType.TYPE_NUMBER_FLAG_DECIMAL | InputType.TYPE_NUMBER_FLAG_SIGNED |
                        InputType.TYPE_CLASS_NUMBER);
                input.setRawInputType(Configuration.KEYBOARD_12KEY);
                alert.setView(input);
                alert.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        //Put actions for OK button here

                        // todo: catch exception if user input cannot be parsed ...
                        MessagePayload payload = null;
                        switch (vItem.mDef.mDataType) {
                            case Float:
                                payload = new MessagePayload(Float.parseFloat(input.getText().toString()));
                                break;
                            case Int:
                                payload = new MessagePayload(Integer.parseInt(input.getText().toString()));
                                break;
                            default:
                                Log.e(LogTag, "Unsupported channel data type");
                        }

                        if (payload != null) {
                            Message msg = new Message(Message.MessageType.INPUT, vItem.mDef.mNumber,
                                    payload);

                            SerialInterface si = new SerialInterface();
                            String sMsg = si.generateOutput(msg);
                            BTSerial.INSTANCE.send(sMsg);
                        }
                    }
                });
                alert.setNegativeButton("cancel", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        //Put actions for CANCEL button here, or leave in blank
                    }
                });
                alert.show();
            }
        });

        // listen on channel changes
        BTSerial.INSTANCE.subscribeChannelOutput(this);

        return rootView;
    }

    public void updateChannel(int channel, MessagePayload payload) {

        boolean channelSet = false;
        for (int i = 0; i < mAdapter.getCount(); i++) {
            ChannelView v = mAdapter.getItem(i);
            if (v.mDef.mNumber == channel) {
                if (payload.m_type == Message.MessagePayloadType.FLOAT) {
                    v.mValue.setFloatValue(payload.m_float);
                }
                if (payload.m_type == Message.MessagePayloadType.INT) {
                    v.mValue.setIntValue(payload.m_int);
                }

                channelSet = true;
            }
        }

        if (channelSet) {
            mAdapter.notifyDataSetChanged();
        } else {
            Log.e("acontrol", "Channel " + channel + " not in Channel definition");

        }

    }

    @Override
    public void change(int channel, MessagePayload payload) {
        updateChannel(channel, payload);
    }
}

