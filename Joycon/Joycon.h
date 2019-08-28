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

#include <iostream>
#include <stdio.h>
#include <vector>
#include <thread>
#include <mutex>
#include <deque>
#include <functional>
#include <assert.h>
#include "enums.h"
#include "constants.h"

// Actual class.
// Abstracts away as much as possible.
class EXPORT_DECL Joycon
{
public:
    Joycon(void *handle_, JOYCON_TYPE type_, int num, const char *hostmac_);
    int Start(JOYCON_SCHEMA schema, bool enable_rumble);

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