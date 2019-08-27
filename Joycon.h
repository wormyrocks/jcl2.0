#ifndef _JOYCON
#define _JOYCON
#include <run.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define i16 int16_t
#define i32 int32_t

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
    enum ReportType
    {
        RT_3F,
        RT_21,
        RT_30,
        RT_31,
        RT_END
    };
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
    const string button_names[BUTTONS_END] = {
        "Y", "X", "B", "A", "L_SR", "L_SL", "R", "ZR", "-", "+", "RS", "LS", "HOME", "CAPTURE", "PAPERCLIP", "GRIP", "DOWN", "UP", "RIGHT", "LEFT", "R_SR", "R_SL", "L", "ZL"};
    enum GyroScale
    {
        GS_250DPS,
        GS_500DPS,
        GS_1000DPS,
        GS_2000DPS,
        GS_END
    };
    enum AccelScale
    {
        AS_8G,
        AS_4G,
        AS_2G,
        AS_16G,
        AS_END
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

    enum SubcommandType
    {
        SC_NOTHING = 0x00,
        SC_REQUEST_DEVICE_INFO = 0x02,
        SC_SET_INPUT_REPORT_MODE = 0x03,
        SC_SPI_FLASH_READ = 0x10,
        SC_SPI_FLASH_WRITE = 0x11,
        SC_SET_PLAYER_LIGHTS = 0x30,
        SC_GET_PLAYER_LIGHTS = 0x31,
        SC_SET_HOME_LIGHT = 0x38,
        SC_TOGGLE_IMU = 0x40,
        SC_SET_IMU_SENSITIVITY = 0x41,
        SC_ENABLE_VIBRATION = 0x48,
        SC_GET_VOLTAGE = 0x50,
        SC_END
    };
    enum ToggleParam
    {
        TP_IMU = SC_TOGGLE_IMU,
        TP_RUMBLE = SC_ENABLE_VIBRATION,
        TP_END
    };
    const u8 report_type_names[RT_END] = {0x3f, 0x21, 0x30, 0x31};
    //constructor
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
    void SetIMUSensitivity(GyroScale gs, AccelScale as, GyroRate gr, AccelFilter af);
    u16 GetBatteryLevel();
    float GetBatteryLevelFloat();
    float *GetRawAccel() { return acc_g; };
    float *GetRawGyro() { return gyr_dps; };

private:
    // joycon.cpp
    void jcSetup();
    void jcSendEmpty();

    // c functions in helpers.h
    void finish();
    void process();
    void process_imu();
    void subcomm(u8 *in, u8 len, SubcommandType subcomm, u8 get_response);
    void comm(u8 *in, u8 len, SubcommandType subcomm, u8 get_response, u8 command);
    bool comm(u8 *in, u8 len, SubcommandType subcomm, u8 get_response, u8 command, u8 silent);
    int hid_read_buffer(bool silent, bool block);
    u8 *read_spi(u32 addr, int len);
    void get_stick_cal();
    void get_imu_cal();
    void setup_joycon(u8 leds);
    void set_report_type(u8 val);
    void update_imu_cal_multipliers(AccelScale as, GyroScale gs);

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
    volatile float stick[2];
    string hostmac;
    JCType jtype;
    hid_device *jc = NULL;
    bool kill_threads = false;

    // Send and receive buffers
    u8 data[DATA_BUFFER_SIZE];
    u8 buf[OUT_BUFFER_SIZE];

    // Calibration
    u16 stick_cal[8];

    // Constant values describing accel multipliers
    // multipliers for 8G, 4G, 2G, 16G
    const i16 AccelScaleCoeff[4] = {8, 4, 2, 16};
    const i16 GyroScaleCoeff[4] = {250, 500, 1000, 2000};
    
    // Accel zero / neutral / sensitivity values from SPI
    // accel_cal [0] = origin x
    // accel_cal [1] = origin y
    // accel_cal [2] = origin z
    // accel_cal [3] = multiplier x
    // accel_cal [4] = multiplier y
    // accel_cal [5] = multiplier z

    float accel_scale_factor = 1;
    i16 accel_cal[6];
    i16 gyr_cal[6];

    // Accelerometer horizontal offsets
    i16 accel_offset[3];

    // Accelerometer multiplier (calculated from previous values)
    float accel_multiplier[3];
    float gyro_multiplier[3];

    // IMU data
    float acc_g[3];
    float gyr_dps[3];

    u8 packet_count = 0;
    u8 timestamp = 0;
    int jc_num = 0;
    bool rumble_enabled = true; // TODO : get rumble status on start
    bool imu_enabled = true;
    u16 batteryLevel = 0;
    // TODO: add approximate battery level

    // State variables
    volatile ReportType report_type;
    // Button states
    volatile u32 rbuttons = 0;
    volatile u32 dbuttons = 0;
    volatile u32 buttons = 0;
};
#endif
