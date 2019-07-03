#ifndef _JOYCON
#define _JOYCON
#include <run.h>

#define u8 uint8_t
#define u16 uint16_t

#define NINTENDO_ID 0x057e
#define JOYCON_L_ID 0x2006
#define JOYCON_R_ID 0x2007
#define JOYCON_P_ID 0x2008

#define DATA_BUFFER_SIZE 49
#define OUT_BUFFER_SIZE 49

class JoyCon
{
public:
    //commands
    JoyCon(hid_device *handle, JCType jtype, char *hostmac);
    void Begin();
    void Cleanup();

    //getters and setters
    bool isConnected();

    //one liners
    bool isLeft() { return (jtype == JCType::LEFT); }
    bool isRight() { return (jtype == JCType::RIGHT); }
    bool isPro() { return (jtype == JCType::PRO); }
    hid_device *getHidDevice() { return jc; };

    // only private because of thread callback
    void jcLoop();

private:
    // joycon.cpp
    void jcSetup();
    void jcSendEmpty();

    // c functions in helpers.h
    void subcomm(hid_device *joycon, u8 *in, u8 len, u8 subcomm, u8 get_response);
    void comm(hid_device *joycon, u8 *in, u8 len, u8 subcomm, u8 get_response, u8 command);
    u8 *read_spi(hid_device *jc, u8 addr1, u8 addr2, int len);
    void get_stick_cal(hid_device *jc);
    void setup_joycon(hid_device *jc, u8 leds);

    // thread stuff
    thread jcloop;
    volatile bool do_kill = false;

    string hostmac;
    JCType jtype;
    hid_device *jc = NULL;
    unsigned short left_buttons = 0;
    unsigned short right_buttons = 0;
    bool kill_threads = false;
    u8 data[DATA_BUFFER_SIZE];
    u16 stick_cal[14];
    u8 packet_count = 0;
};
#endif
