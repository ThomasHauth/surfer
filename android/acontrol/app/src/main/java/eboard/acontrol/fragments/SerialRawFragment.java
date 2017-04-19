package eboard.acontrol.fragments;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;

import eboard.acontrol.R;
import eboard.serial.BTSerial;
import eboard.serial.ISerialMessageReceiver;

public class SerialRawFragment extends Fragment implements ISerialMessageReceiver {
    TextView mRawSerial = null;
    ScrollView mRawSerialScroll = null;
    CheckBox mAutoScroll = null;
    EditText mEditText = null;
    Button mSendButton = null;

    public SerialRawFragment() {
    }

    /**
     * Returns a new instance of this fragment for the given section
     * number.
     */
    public static SerialRawFragment newInstance() {
        SerialRawFragment fragment = new SerialRawFragment();

        return fragment;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.serial_raw_acontrol, container, false);
        mRawSerial = (TextView) rootView.findViewById(R.id.raw_serial);
        mRawSerialScroll = (ScrollView) rootView.findViewById(R.id.raw_serial_scroll);
        mAutoScroll = (CheckBox) rootView.findViewById(R.id.raw_serial_autoscroll);
        mEditText = (EditText) rootView.findViewById(R.id.raw_serial_cmd);
        mSendButton = (Button) rootView.findViewById(R.id.raw_serial_send);

        mSendButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                BTSerial.INSTANCE.send(mEditText.getText().toString());
            }
        });

        BTSerial.INSTANCE.subscribeMessage(this);

        return rootView;
    }

    @Override
    public void recv(String message) {
        if (mRawSerial != null) {
            mRawSerial.append(message + "\n");
            // auto-scroll to bottom
            if (mAutoScroll.isChecked()) {
                mRawSerialScroll.smoothScrollTo(0, mRawSerial.getBottom());
            }

            if (mRawSerial.getLineCount() > 1000) {
                mRawSerial.setText("");
            }
        }
    }
}