package eboard.acontrol;

import android.test.AndroidTestCase;

import java.util.List;

import eboard.serial.Message;
import eboard.serial.MessagePayload;
import eboard.serial.SerialInterface;

public class SerialInterfaceTest extends AndroidTestCase {

    public void testSuccess(){
        assertTrue(true);
    }

    public void testParseMessageNop(){
        String sMessage = "%1%1%3%0%";
        SerialInterface si = new SerialInterface();

        List<Message> msgs = si.parseInput(sMessage);

        assertEquals(1, msgs.size());
        assertEquals(Message.MessageType.NOP, msgs.get(0).m_type);
    }

    public void testParseMessageInputFloat(){
        String sMessage = "%2%5%2%99.9%";
        SerialInterface si = new SerialInterface();

        List<Message> msgs = si.parseInput(sMessage);

        assertEquals(1, msgs.size());
        assertEquals( Message.MessageType.INPUT, msgs.get(0).m_type);
        assertEquals(5,msgs.get(0).m_code);
        assertEquals( Message.MessagePayloadType.FLOAT, msgs.get(0).m_payload.m_type);
        assertEquals(99.9, msgs.get(0).m_payload.m_float, 0.1);
    }

    public void testParseMessageOutputInt(){
        String sMessage = "%3%5%1%99%";
        SerialInterface si = new SerialInterface();

        List< Message> msgs = si.parseInput(sMessage);

        assertEquals(1, msgs.size());
        assertEquals( Message.MessageType.OUTPUT, msgs.get(0).m_type);
        assertEquals(5,msgs.get(0).m_code);
        assertEquals( Message.MessagePayloadType.INT, msgs.get(0).m_payload.m_type);
        assertEquals(99, msgs.get(0).m_payload.m_int);
    }

    public void testGenerateOutputFloat() {
        SerialInterface si = new SerialInterface();
         Message msg = new  Message(Message.MessageType.OUTPUT,
                23, new MessagePayload(100.0f)  );

        String sout = si.generateOutput(msg);
        assertEquals("%3%23%2%100.0%", sout);
    }

    public void testGenerateOutpuIntt() {
        SerialInterface si = new SerialInterface();
        Message msg = new  Message(Message.MessageType.STREAM,
                1,  new MessagePayload(-100)  );

        String sout = si.generateOutput(msg);
        assertEquals("%4%1%1%-100%", sout);
    }

}