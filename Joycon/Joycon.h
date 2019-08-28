#ifndef _JOYCON_H
#define _JOYCON_H
#ifdef BUILDING_SHARED_LIB
#include <hidapi.h>
#ifdef _WIN32
#define EXPORT_DECL __declspec(dllexport)
#else
#define EXPORT_DECL
#endif
#else
#ifdef _WIN32
#define EXPORT_DECL __declspec(dllimport)
#else
#define EXPORT_DECL
#endif
#endif

#include "enums.h"
#include "constants.h"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <thread>
#include <mutex>
#include <deque>
#include <functional>
#include <assert.h>

// Wrapper class.
// Provides public functions accessible to people using the DLL
class EXPORT_DECL JoyCon
{
public:
    JoyCon(void *_jcobj);

private:
    const void *myobj;
};

// Actual class.
// Abstracts away as much as possible.
class JoyConObj
{
public:
    JoyConObj(void *hidapi_handle, JOYCON_TYPE jtype, int number, const char *hostmac);
    int Start(JOYCON_SCHEMA schema, bool enable_rumble);
    JoyCon *interface;

private:
    std::thread jcloop;
    volatile bool do_kill;
    std::mutex *datamtx;
    std::mutex *cmdmtx;
    std::deque<std::function<void()>> fq;
    volatile float stick[2];
    std::string hostmac;
    JOYCON_TYPE jtype;
    void *hidapi_handle = NULL;
    bool kill_threads = false;
    int jc_num = 0;
};
#endif