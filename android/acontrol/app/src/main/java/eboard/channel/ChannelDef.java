/**
 * This file has been auto-generated, don't modify it.
 */

package eboard.channel;

import java.util.Arrays;
import java.util.List;

public class ChannelDef {
public static int raw_adc_1 = 1;
public static int smoothed_adc_1 = 2;
public static int accel_exponent = 3;
public static int accel_min = 4;
public static int accel_max = 5;
public static int speed_max = 6;
public static int motor_1_speed_set = 7;
public static int motor_1_speed_actual = 8;
public static int bat1_s1 = 9;
public static int bat1_s2 = 10;
public static int bat1_s1s2 = 11;
public static List<Channel> mChannels = Arrays.asList( 
new Channel(1,"raw_adc_1", "RAW ADC1 output", Channel.DataType.Float),
new Channel(2,"smoothed_adc_1", "Smoothed ADC1 output", Channel.DataType.Float),
new Channel(3,"accel_exponent", "The exponent of the function limiting the motor speedup (0 to inf)", Channel.DataType.Float),
new Channel(4,"accel_min", "The initial acceleration per second (0 to 1)", Channel.DataType.Float),
new Channel(5,"accel_max", "The maximim acceleration per second (0 to 1)", Channel.DataType.Float),
new Channel(6,"speed_max", "The maximim speed the controller will send to the motor (0 to 1)", Channel.DataType.Float),
new Channel(7,"motor_1_speed_set", "Set Speed of motor 1 ( 0 to 1)", Channel.DataType.Float),
new Channel(8,"motor_1_speed_actual", "Actual Speed of motor 1 set to controller (0 to 1)", Channel.DataType.Float),
new Channel(9,"bat1_s1", "Voltage of Battery 1 Cell 1 (full 4.2V)", Channel.DataType.Float),
new Channel(10,"bat1_s2", "Voltage of Battery 1 Cell 2 (full 4.2V)", Channel.DataType.Float),
new Channel(11,"bat1_s1s2", "Voltage of Battery 1 Cell 1+2 (full 8.4V)", Channel.DataType.Float)
 ); 
 }