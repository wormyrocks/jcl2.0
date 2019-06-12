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
    JoyCon(hid_device *handle, JCType type);
    void joycon_main_loop();
    bool isConnected();

    //one liners
    bool isLeft() { return (type == JCType::LEFT); }
    bool isRight() { return (type == JCType::RIGHT); }
    bool isPro() { return (type == JCType::PRO); }
    hid_device *getHidDevice() { return jc; };

private:
    JCType type;
    hid_device *jc = NULL;
    USHORT left_buttons = 0;
    USHORT right_buttons = 0;
    bool kill_threads = false;
    u8 data[DATA_BUFFER_SIZE];
    u16 stick_cal[14];
    u8 global_counter = 0;
};
#endif