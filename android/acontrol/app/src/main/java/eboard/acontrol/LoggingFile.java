package eboard.acontrol;

import android.content.Context;
import android.location.Location;
import android.os.Environment;
import android.util.Log;
import android.util.Xml;

import org.xmlpull.v1.XmlSerializer;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

import eboard.channel.Channel;
import eboard.channel.ChannelBuffer;
import eboard.channel.ChannelDef;
import eboard.serial.MessagePayload;

/**
 * This class stores the application state to files on the SD card. It can store
 * the content of all channels and the current GPS position
 * in a file of format GPX via the log() method.
 */
public class LoggingFile {

    /**
     * FileWriter for the gps locations file
     */
    private FileWriter mLocationsFileWriter;

    /**
     * FileWriter for the Channels values file
     */
    private FileWriter mChannelsFileWriter;

    /**
     * The tag used with the android logging system
     */
    private String LogTag = "acontrol.LoggingFile";

    /**
     * File the channel information is written to
     */
    private File mChannelsFile = null;

    /**
     * File the gps location information is written to
     */
    private File mLocationsFile = null;

    /**
     * Xml ouput class to store the GPS location information
     */
    private XmlSerializer mLocationsFileSer = null;

    /**
     * This construction opens new logging files which have the date and time
     * in the file name so every time the application is started a new log file
     * gets created.
     */
    LoggingFile(Context ctx) {

        // Generate the file name which uses the current date & time
        String datename = "eboard_" + android.text.format.DateFormat.format("yyyy-MM-dd_hh-mm-ss", new java.util.Date()).toString();
        File mPath = Environment.getExternalStoragePublicDirectory("/eboard");
        Log.i(LogTag, "Using storage directory " + mPath.toString() + "  " + Environment.DIRECTORY_MOVIES);
        mPath.mkdirs();

        mChannelsFile = new File(mPath, datename + ".log");
        mLocationsFile = new File(mPath, datename + ".gpx");
        mLocationsFileSer = Xml.newSerializer();

        try {
            if (!mChannelsFile.createNewFile()) {
                Log.e(LogTag, "File " + mChannelsFile.getName() + "cannot be created");
                return;
            }

            mChannelsFileWriter = new FileWriter(mChannelsFile);

            if (!mLocationsFile.createNewFile()) {
                Log.e(LogTag, "File " + mLocationsFile.getName() + "cannot be created");
                return;
            }

            mLocationsFileWriter = new FileWriter(mLocationsFile);
            mLocationsFileSer.setOutput(mLocationsFileWriter);

            writeChannelHeader();
            writeGpxHeader();

        } catch (IOException e) {
            e.printStackTrace();
            return;
        }

        mIsAvailable = true;
        Log.i(LogTag, "Logging started with file name " + datename);
    }

    /**
     * Write the header of the gpx file, needs to be called before any call to
     * wirteLocation()
     */
    private void writeGpxHeader() throws IOException {
        XmlSerializer s = mLocationsFileSer;

        s.startDocument("UTF-8", true);

        s.startTag("", "gpx");
        s.attribute("", "xmlns", "http://www.topografix.com/GPX/1/1");
        s.attribute("", "version", "1.1");
        s.attribute("", "creator", "eboard android control");
        s.attribute("xmlns", "xsi", "http://www.w3.org/2001/XMLSchema-instance");
        s.attribute("xsi", "schemaLocation", "http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd");

        s.startTag("", "trk");
        s.startTag("", "name");
        s.text("eboard Track one");
        s.endTag("", "name");
        s.startTag("", "trkseg");

        mLocationsFileSer.flush();
    }

    /**
     * Write the header of the channel output file.
     */
    private void writeChannelHeader() throws IOException {

        String headerString = "";
        for (Channel c : ChannelDef.mChannels) {
            headerString += c.mName + ",";
        }

        // drop last comma
        headerString = headerString.substring(0, headerString.length() - 1);

        mChannelsFileWriter.write(headerString + "\n");

    }

    /**
     * Write the Footer of the GPX files. This needs to be called
     * after the last call to log()
     */
    private void writeGpxFooter() throws IOException {
        XmlSerializer s = mLocationsFileSer;

        s.endTag("", "trkseg");
        s.endTag("", "trk");
        s.endTag("", "gpx");
        s.endDocument();
        s.flush();
    }

    /**
     * Checks if external storage is available for read and write
     */
    public boolean isExternalStorageWritable() {
        String state = Environment.getExternalStorageState();
        if (Environment.MEDIA_MOUNTED.equals(state)) {
            return true;
        }
        return false;
    }

    /**
     * Close all output files
     */
    public void close() {
        try {
            mChannelsFileWriter.flush();
            mChannelsFileWriter.close();

            writeGpxFooter();

            mLocationsFileWriter.flush();
            mLocationsFileWriter.close();

        } catch (IOException e) {
            Log.e(LogTag, "Logging files could not be properly closed");
            e.printStackTrace();
        }
        finally {
            mIsAvailable = false;
        }
    }

    /**
     * True if the output files have been properly opened for writing
     */
    boolean mIsAvailable = false;

    /**
     * Returns true if the output files have be properly opened for writing
     */
    boolean isAvailable() {
        return mIsAvailable;
    }


    /**
     * Store the current content of the channel buffer and the current GPS location
     * into the two output files.
     */
    public void log(ChannelBuffer mChannelBuffer, Location mLastLocation) {
        try {

            if (!isAvailable())
                return;

            // get all channels

            String csvLine = "";
            for (Channel c : ChannelDef.mChannels) {
                MessagePayload msgp = mChannelBuffer.getBuffered(c.mNumber);
                if (msgp == null) {
                    csvLine += "undef,";
                } else {
                    csvLine += msgp.stringValue() + ",";
                }
            }

            // drop last comma
            csvLine = csvLine.substring(0, csvLine.length() - 1);

            mChannelsFileWriter.write(csvLine + "\n");
            mChannelsFileWriter.flush();

            writeLocation(mLastLocation);

        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    /**
     * Write the GPS location into the opened GPX file
     */
    private void writeLocation(Location mLastLocation) throws IOException {

        if (mLastLocation == null)
            return;

        XmlSerializer s = mLocationsFileSer;
        s.startTag("", "trkpt");
        s.attribute("", "lat", new Double(mLastLocation.getLatitude()).toString());
        s.attribute("", "lon", new Double(mLastLocation.getLongitude()).toString());
        s.endTag("", "trkpt");
    }
}
