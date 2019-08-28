#ifndef _ENUMS_H
#define _ENUMS_H

enum JoyconType : int
{
    LEFT = 0,
    RIGHT = 1,
    PRO = 2,
    ANY = 3
};

enum JoyconSchema : int
{
    SCHEMA_NOCONFIG,
    SCHEMA_SIMPLE_INPUT,
    SCHEMA_GAMEPAD,
    SCHEMA_IMU,
    SCHEMA_CAMERA,
};

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

enum CallbackType
{
    JOYCON_CALLBACK_CONNECTED,
    JOYCON_CALLBACK_NEWDATA,
    JOYCON_CALLBACK_NEWINPUT,
    JOYCON_CALLBACK_LENGTH
};

#endif