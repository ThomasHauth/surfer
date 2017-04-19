package eboard.acontrol.fragments;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ProgressBar;

import eboard.acontrol.R;
import eboard.channel.ChannelDef;
import eboard.serial.BTSerial;
import eboard.serial.Message;
import eboard.serial.MessagePayload;
import eboard.serial.SerialInterface;

public class SpeedControlFragment extends Fragment {
    ProgressBar mSpeedSlider = null;
    Button mMaxSpeed = null;
    Button mMinSpeed = null;
    Button mHMaxSpeed = null;
    Button mHMinSpeed = null;
    Button mZeroSpeed = null;

    public SpeedControlFragment() {
    }

    /**
     * Returns a new instance of this fragment for the given section
     * number.
     */
    public static SpeedControlFragment newInstance() {
        SpeedControlFragment fragment = new SpeedControlFragment();

        return fragment;
    }

    private void  setSpeed (float f) {
        MessagePayload payload = new MessagePayload(f);
        Message msg = new Message(Message.MessageType.INPUT, ChannelDef.motor_1_speed_set,
                payload);

        SerialInterface si = new SerialInterface();
        String sMsg = si.generateOutput(msg);
        BTSerial.INSTANCE.send(sMsg);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.speed_control_fragment, container, false);

        final float[] lastRatio = {0};

        mSpeedSlider = (ProgressBar) rootView.findViewById(R.id.speedSlider);

        mSpeedSlider.setOnTouchListener(new View.OnTouchListener() {

            public boolean onTouch(View view, MotionEvent event) {
                float ratio = 0.0f;
                if ((event.getAction() == MotionEvent.ACTION_DOWN) || (event.getAction() == MotionEvent.ACTION_MOVE)) {
                    ratio = event.getY() / view.getHeight();

                    ratio = Math.max(ratio, 0.0f);
                    ratio = Math.min(ratio, 1.0f);
                } else if (event.getAction() == MotionEvent.ACTION_UP) {
                    // reset to zero because the user lifted finger
                    ratio = 0.0f;
                }

                if (Math.abs(ratio - lastRatio[0]) > 0.05f) {
                    lastRatio[0] = ratio;
                    setSpeed(ratio);
                }
                return true;
            }
        });

        mMaxSpeed = (Button) rootView.findViewById(R.id.maxSpeed);
        mZeroSpeed = (Button) rootView.findViewById(R.id.zeroSpeed);
        mMinSpeed = (Button) rootView.findViewById(R.id.minSpeed);
        mHMaxSpeed = (Button) rootView.findViewById(R.id.halfMaxSpeed);
        mHMinSpeed = (Button) rootView.findViewById(R.id.halfMinSpeed)
        ;
        mMaxSpeed.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                setSpeed(1.0f);
            }
        });

        mZeroSpeed.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                setSpeed(0.5f);
            }
        });

        mMinSpeed.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                setSpeed(0.0f);
            }
        });

        mHMinSpeed.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                setSpeed(0.25f);
            }
        });
        mHMaxSpeed.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                setSpeed(0.75f);
            }
        });

        return rootView;
    }


}
