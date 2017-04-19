package eboard.acontrol;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;

import java.util.Locale;

import eboard.acontrol.fragments.ChannelFragment;
import eboard.acontrol.fragments.SerialRawFragment;
import eboard.acontrol.fragments.SpeedControlFragment;

/**
 * Fragment page adapter which holds the three fragments for the tabs
 * of the main activity.
 */
public class SectionsPagerAdapter extends FragmentPagerAdapter {

    public SectionsPagerAdapter(FragmentManager fm) {
        super(fm);
    }

    @Override
    public Fragment getItem(int position) {
        // getItem is called to instantiate the fragment for the given page.
        // Return a PlaceholderFragment (defined as a static inner class below).
        if (position == 0) {
            return SpeedControlFragment.newInstance();
        } else if (position == 1) {
            return ChannelFragment.newInstance();
        } else if (position == 2) {
            return SerialRawFragment.newInstance();
        }
        return null;
    }

    @Override
    public int getCount() {
        // Show 3 total pages.
        return 3;
    }

    @Override
    public CharSequence getPageTitle(int position) {
        Locale l = Locale.getDefault();
        switch (position) {
            case 0:
                return "CONTROL";// getString(R.string.title_section_control).toUpperCase(l);
            case 1:
                return "CHANNELS";// getString(R.string.title_section_channels).toUpperCase(l);
            case 2:
                return "RAW"; // getString(R.string.title_section_serial_raw).toUpperCase(l);
        }
        return null;
    }
}

