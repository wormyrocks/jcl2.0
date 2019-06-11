#ifndef _RUN
#define _RUN

using namespace std;

#ifdef _WIN32
#include "Windows.h"
#endif

#include <string>
#include <iostream>
#include <thread>
#include <hidapi.h>
#include <csignal>
#include "grabmac.h"

#define u8 uint8_t
#define u16 uint16_t

const unsigned short NINTENDO = 1406; // 0x057e
const unsigned short JOYCON_L = 8198; // 0x2006
const unsigned short JOYCON_R = 8199; // 0x2007

#define DATA_BUFFER_SIZE 49
#define OUT_BUFFER_SIZE 49
u8 data[DATA_BUFFER_SIZE];
u16 stick_cal[14];
u8 global_counter[2] = { 0,0 };
char * mac_addr;

hid_device *left_joycon = NULL;
hid_device *right_joycon = NULL;
int res;
USHORT left_buttons = 0;
USHORT right_buttons = 0;
bool kill_threads = false;


#endif