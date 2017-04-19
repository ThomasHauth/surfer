package eboard.acontrol;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.Intent;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.view.ViewPager;
import android.support.v7.app.ActionBar;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;

import eboard.channel.ChannelBuffer;
import eboard.serial.BTSerial;

/**
 * Main activity for the acontrol application. This is a multi-tab activity using fragments
 * with one tabs for some high-level control of channel values, one tab for listing all
 * channel values and one tab for raw serial communication log
 */
public class acontrol extends ActionBarActivity implements ActionBar.TabListener {

    /**
     * Holds a fragment for each of the three tabs
     */
    SectionsPagerAdapter mSectionsPagerAdapter;

    ViewPager mViewPager;

    /**
     * Tag to be use for logging output
     */
    String LogTag = "acontrol";

    /**
     * Bluetooth adapter used to communicate with the arduino device
     */
    BluetoothAdapter mBluetoothAdapter = null;

    /**
     * Handle for the connected bluetooth arduino device
     */
    BluetoothDevice mBluetoothDevice = null;

    /**
     * Communication socket for serial i/o over bluetooth
     */
    BluetoothSocket mSocket = null;

    /**
     * BT output stream
     */
    OutputStream mOutputStream = null;

    /**
     * BT input stream
     */
    InputStream mInputStream = null;

    /**
     * byte buffer for reading BT input data. Declare as class member as it is used by a async
     * thread
     */
    byte[] mInputThread_readBuffer;
    /**
     * Current position on the read input buffer. Declare as class member as it is used by a async
     * thread
     */
    int mInputThread_readBufferPosition;

    /**
     * Set to true, if he input reader worker thread should terminate
     */
    volatile boolean mInputThread_stop;

    /**
     * worker thread with asynchronously pulls the data from the serial bluetooth
     */
    Thread mInputThread;

    /**
     * Class which buffers all channel values which have been transferred via bluetooth
     */
    ChannelBuffer mChannelBuffer;

    /**
     * Task which logs the channel content and GPS location to storage
     */
    private LoggingTimerTask mLoggingTimerTask;

    /**
     * This timer triggers the mLoggingTimeTask every second
     */
    private Timer mLoggingTimer;

    /**
     * This logging file class holds handles to the channel content and GPX file
     * and is used to fill these files on a periodic basis
     */
    LoggingFile mLogFile = null;

    /**
     * The last GPS location which has been retrieved
     */
    Location mLastLocation = null;

    /**
     * Stops the activity and the state logging
     */
    @Override
    protected void onStop()
    {
        super.onStop();
        stopLogging();
    }

    /**
     * Create the activity and establish the connection via bluetooth to the arduino
     * chip
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_acontrol);

        // Set up the action bar.
        final ActionBar actionBar = getSupportActionBar();
        actionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_TABS);

        // Create the adapter that will return a fragment for each of the three
        // primary sections of the activity.
        mSectionsPagerAdapter = new SectionsPagerAdapter(getSupportFragmentManager());

        // Set up the ViewPager with the sections adapter.
        mViewPager = (ViewPager) findViewById(R.id.pager);
        mViewPager.setAdapter(mSectionsPagerAdapter);

        // When swiping between different sections, select the corresponding
        // tab. We can also use ActionBar.Tab#select() to do this if we have
        // a reference to the Tab.
        mViewPager.setOnPageChangeListener(new ViewPager.SimpleOnPageChangeListener() {
            @Override
            public void onPageSelected(int position) {
                actionBar.setSelectedNavigationItem(position);
            }
        });

       /* mRawSerial = (TextView)
                mViewPager.findViewById(R.id.raw_serial);
*/


        // For each of the sections in the app, add a tab to the action bar.
        for (int i = 0; i < mSectionsPagerAdapter.getCount(); i++) {
            // Create a tab with text corresponding to the page title defined by
            // the adapter. Also specify this Activity object, which implements
            // the TabListener interface, as the callback (listener) for when
            // this tab is selected.
            actionBar.addTab(
                    actionBar.newTab()
                            .setText(mSectionsPagerAdapter.getPageTitle(i))
                            .setTabListener(this));
        }


        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null) {
            Log.e(LogTag, "No bluetooth adapter available");
        }

        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableBluetooth = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBluetooth, 0);
        }

        Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();
        Log.i(LogTag, "Found paired bluetooth device(s) " + pairedDevices.size());
        if (pairedDevices.size() > 0) {
            for (BluetoothDevice device : pairedDevices) {
                if (device.getName().equals("HC-05")) {
                    mBluetoothDevice = device;
                    // break;
                }
                Log.i(LogTag, "Blueetooth device with name " + device.getName());
            }
        }

        if (mBluetoothDevice != null) {
            Log.i(LogTag, "Arduino Bluetooth device found");
        } else {
            Log.e(LogTag, "Arduino Bluetooth device not found");
        }

        if (mBluetoothDevice != null ) {
            // only access Arduino bt device, if it has been found
            UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"); //Standard SerialPortService ID
            try {
                mSocket = mBluetoothDevice.createRfcommSocketToServiceRecord(uuid);
                if (mSocket == null) {
                    Log.e(LogTag, "Cannot open bluetooth socket");
                }
                mSocket.connect();
                mOutputStream = mSocket.getOutputStream();
                mInputStream = mSocket.getInputStream();

                BTSerial.INSTANCE.setOutputStream(mOutputStream);
            } catch (IOException e) {
                e.printStackTrace();
            }


            beginListenForData();

            //myLabel.setText("Bluetooth Opened");
            beginListenForGPS();

            startLogging();

            // register buffer for channel values
            mChannelBuffer = new ChannelBuffer();
            BTSerial.INSTANCE.subscribeChannelOutput(mChannelBuffer);
        }
    }

    /**
     * This task stores the current content of all channels and the gps location
     * to the permanent storage.
     */
    class LoggingTimerTask extends TimerTask {
        public void run() {
            mLogFile.log(mChannelBuffer,mLastLocation );
        }
    }

    /**
     * Start logging the application state to storage
     */
    void startLogging(){
        mLogFile = new LoggingFile(this);


        mLoggingTimerTask = new LoggingTimerTask();
        mLoggingTimer = new Timer();

        mLoggingTimer.schedule(mLoggingTimerTask, 1000, 1000);
    }

    /**
     * Stop logging the application state to storage
     */
    void stopLogging() {
        if (mLogFile != null) {
            mLogFile.close();
            mLogFile = null;
        }

        if ( mLoggingTimer != null) {
            mLoggingTimer.cancel();

        }

    }

    /**
     * Install a listener which store the GPS location every time it is updated
     */
    private void beginListenForGPS() {
        final String locationProvider = LocationManager.GPS_PROVIDER;

        final LocationManager mLocationManager = (LocationManager) this.getSystemService(Context.LOCATION_SERVICE);


// Define a listener that responds to location updates
        LocationListener locationListener = new LocationListener() {
            public void onLocationChanged(Location location) {
                // Called when a new location is found by the network location provider.
                //makeUseOfNewLocation(location);
                Log.i(LogTag, "Got new location " + location.toString());
                mLastLocation = location;
            }

            public void onStatusChanged(String provider, int status, Bundle extras) {}

            public void onProviderEnabled(String provider) {
                Log.i(LogTag,"Location provider enabled");
            }

            public void onProviderDisabled(String provider) {
                Log.i(LogTag, "Location provider disabled");
            }
        };

        mLocationManager.requestLocationUpdates(locationProvider, 0, 0, locationListener);
    }

    /**
     * Install the input thread which reads data from the BT input stream
     */
    void beginListenForData() {
        final Handler handler = new Handler();
        final byte delimiter = 10; //This is the ASCII code for a newline character

        mInputThread_stop = false;
        mInputThread_readBufferPosition = 0;
        mInputThread_readBuffer = new byte[1024];
        mInputThread = new Thread(new Runnable() {
            public void run() {
                Log.i(LogTag, "Started receiving worker thread");
                while (!Thread.currentThread().isInterrupted() && !mInputThread_stop) {
                    if (mInputStream == null) {
                        // input not available, wait for it...
                        try {
                            Thread.currentThread().sleep(1000);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        continue;
                    }
                    try {
                        int bytesAvailable = mInputStream.available();
                        if (bytesAvailable > 0) {
                            byte[] packetBytes = new byte[bytesAvailable];
                            mInputStream.read(packetBytes);
                            for (int i = 0; i < bytesAvailable; i++) {
                                byte b = packetBytes[i];
                                if (b == delimiter) {
                                    byte[] encodedBytes = new byte[mInputThread_readBufferPosition];
                                    System.arraycopy(mInputThread_readBuffer, 0, encodedBytes, 0, encodedBytes.length);
                                    final String data = new String(encodedBytes, "US-ASCII");
                                    mInputThread_readBufferPosition = 0;

                                    handler.post(new Runnable() {
                                        public void run() {
                                            //myLabel.setText(data);
                                            //Log.i(LogTag, "Recv: " + data);
                                            BTSerial.INSTANCE.recvMessage(data);
                                            //mRawSerial.append( data );
                                        }
                                    });
                                } else {
                                    mInputThread_readBuffer[mInputThread_readBufferPosition++] = b;
                                }
                            }
                        }
                    } catch (IOException ex) {
                        mInputThread_stop = true;
                    }
                }
            }
        });

        mInputThread.start();
    }


    /**
     * Provide the default options menu
     */
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_acontrol, menu);
        return true;
    }

    /**
     * Use defaults for options items selection
     */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    /**
     * Forward tab selection to the view pager, which shows the proper fragment
     */
    @Override
    public void onTabSelected(ActionBar.Tab tab, FragmentTransaction fragmentTransaction) {
        // When the given tab is selected, switch to the corresponding page in
        // the ViewPager.
        mViewPager.setCurrentItem(tab.getPosition());
    }

    /**
     * No special action required
     */
    @Override
    public void onTabUnselected(ActionBar.Tab tab, FragmentTransaction fragmentTransaction) {
    }

    /**
     * No special action required
     */
    @Override
    public void onTabReselected(ActionBar.Tab tab, FragmentTransaction fragmentTransaction) {
    }

}
