#ifndef _JOYCON
#define _JOYCON
#include <run.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define NINTENDO_ID 0x057e
#define JOYCON_L_ID 0x2006
#define JOYCON_R_ID 0x2007
#define JOYCON_P_ID 0x2008

#define DATA_BUFFER_SIZE 49
#define OUT_BUFFER_SIZE 49

#define SUBCOMM_ATTEMPTS_NUMBER 8

class JoyCon
{
public:
    enum JOYCON_BUTTONS
    {
        L_Y,
        L_X,
        L_B,
        L_A,
        L_SR,
        L_SL,
        L_R,
        L_ZR,
        S_MINUS,
        S_PLUS,
        S_RSTICK,
        S_LSTICK,
        S_HOME,
        S_CAPTURE,
        S_NULL,
        S_GRIP,
        R_DOWN,
        R_UP,
        R_RIGHT,
        R_LEFT,
        R_SR,
        R_SL,
        R_L,
        R_ZL,
        BUTTONS_END
    };
    enum GyroScale
    {
        GS_250DPS,
        GS_500DPS,
        GS_1000DPS,
        GS_2000DPS
    };
    enum AccelScale
    {
        AS_8G,
        AS_4G,
        AS_2G,
        AS_16G
    };
    enum GyroRate
    {
        GR_833HZ,
        GR_208HZ
    };
    enum AccelFilter
    {
        AF_200HZ,
        AF_100HZ
    };
    const string button_names[BUTTONS_END] = {
        "Y", "X", "B", "A", "L_SR", "L_SL", "R", "ZR", "-", "+", "RS", "LS", "HOME", "CAPTURE", "PAPERCLIP", "GRIP", "DOWN", "UP", "RIGHT", "LEFT", "R_SR", "R_SL", "L", "ZL"};
    enum ReportType
    {
        RT_3F,
        RT_21,
        RT_30,
        RT_31,
        RT_END
    };
    enum ToggleParam
    {
        TP_IMU = 0x40,
        TP_RUMBLE = 0x48,
        TP_END
    };
    const u8 report_type_names[RT_END] = {
        0x3f, 0x21, 0x30, 0x31};
    //commands
    JoyCon(hid_device *handle, JCType jtype, int number, char *hostmac);
    void Cleanup();

    //getters and setters
    bool isConnected();

    //one liners
    bool isLeft() { return (jtype == JCType::LEFT); }
    bool isRight() { return (jtype == JCType::RIGHT); }
    bool isPro() { return (jtype == JCType::PRO); }
    hid_device *getHidDevice() { return jc; };

    // only public because of thread callback
    void jcLoop();

    // queue functions
    void ToggleParameter(ToggleParam tp, bool enable);
    u16 GetBatteryLevel();
    float GetBatteryLevelFloat();

private:
    // joycon.cpp
    void jcSetup();
    void jcSendEmpty();

    // c functions in helpers.h
    void finish();
    void process();
    void subcomm(u8 *in, u8 len, u8 subcomm, u8 get_response);
    void comm(u8 *in, u8 len, u8 subcomm, u8 get_response, u8 command);
    bool comm(u8 *in, u8 len, u8 subcomm, u8 get_response, u8 command, u8 silent);
    int hid_read_buffer(bool silent, bool block);
    u8 *read_spi(u8 addr1, u8 addr2, int len);
    void get_stick_cal();
    void setup_joycon(u8 leds);
    void set_report_type(u8 val);

    // queue functions
    void get_battery_level(std::condition_variable *consume);
    void set_imu_sensitivity(GyroScale gs, AccelScale as, GyroRate gr, AccelFilter af, std::condition_variable *consume);
    void toggle_parameter(ToggleParam tp, bool enable_, std::condition_variable *consume);
    void toggle_parameter(ToggleParam tp, bool enable_) { toggle_parameter(tp, enable_, NULL); };

    // private queue functions (helpers.h)

    // thread stuff
    thread jcloop;
    volatile bool do_kill;
    mutex *datamtx;
    mutex *cmdmtx;

    // internal
    deque<std::function<void()>> fq;
    volatile u32 rbuttons = 0;
    volatile u32 dbuttons = 0;
    volatile u32 buttons = 0;
    volatile float stick[2];
    string hostmac;
    JCType jtype;
    hid_device *jc = NULL;
    bool kill_threads = false;
    u8 data[DATA_BUFFER_SIZE];
    u8 buf[OUT_BUFFER_SIZE];
    u16 stick_cal[7];
    u8 packet_count = 0;
    int jc_num = 0;
    bool rumble_enabled = true; // TODO : get rumble status on start
    bool imu_enabled = true;
    u16 batteryLevel = 0;
    // TODO: add approximate battery level
    volatile ReportType report_type;
};
#endif
