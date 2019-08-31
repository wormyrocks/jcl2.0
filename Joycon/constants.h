#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define NINTENDO_ID 0x057e
#define JOYCON_L_ID 0x2006
#define JOYCON_R_ID 0x2007
#define JOYCON_P_ID 0x2009

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define i16 int16_t
#define i32 int32_t

#define DATA_BUFFER_SIZE 49
#define OUT_BUFFER_SIZE 49

#define HID_READ_TIMEOUT 100
#define CV_TIMEOUT 200

#define SUBCOMM_ATTEMPTS_NUMBER 8

const std::string button_names[BUTTONS_ERROR] = {
    "Y", "X", "B", "A", "L_SR", "L_SL", "R", "ZR", "-", "+", "RS", "LS", "HOME", "CAPTURE", "PAPERCLIP", "GRIP", "DOWN", "UP", "RIGHT", "LEFT", "R_SR", "R_SL", "L", "ZL"};

#endif
