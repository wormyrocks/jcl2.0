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

#include <condition_variable>
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
    Joycon(void *handle_, JoyconType type_, int num, const char *hostmac_, std::function<void(Joycon *)> *callbacks);
    bool Start(JoyconSchema schema);
    bool Stop();
    volatile bool started = false;

    // Threaded functions
    int SetupSchema(JoyconSchema schema);
    int ToggleParameter(ToggleParam tp, bool enable);
    uint16_t GetBatteryLevel();
    float GetBatteryLevelFloat();
    ReportType SetReportType(ReportType type);

private:
    // Threaded functions
    int SetupSchema(JoyconSchema schema, std::condition_variable *consume);
    int ToggleParameter(ToggleParam tp, bool enable, std::condition_variable *consume);
    uint16_t GetBatteryLevel(std::condition_variable *consume);
    ReportType SetReportType(ReportType type, std::condition_variable *consume);

    // This MUST be called after pushing to function queue
    void Unblock();

    bool subcomm(u8 *in, u8 len, SubcommandType subcomm, bool get_response);
    bool comm(u8 *in, u8 len, SubcommandType subcomm, bool get_response, u8 command);
    uint8_t *read_spi(u32 addr, int len);
    int hid_read_buffer(bool do_wait);
    static void threadAdapter(Joycon *j);
    void jcLoop();
    bool process();
    void SendEmpty(); // Wake the main loop if we're not in a continuous polling mode

    // Send and receive buffers
    u8 data[DATA_BUFFER_SIZE];
    u8 outbuf[OUT_BUFFER_SIZE];

    // IMU data
    int last_timestamp;
    float acc_g[3];
    float gyr_dps[3];

    std::function<void(Joycon *)> *callbacks;
    std::thread jcloop;
    volatile bool do_kill;
    volatile bool new_command;
    std::mutex *datamtx;
    std::mutex *killmtx;
    std::mutex *cmdmtx;
    std::deque<std::function<void()>> fq;
    volatile float stick[4];
    std::string hostmac;
    JoyconType jtype;
    void *hidapi_handle = NULL;
    bool kill_threads = false;
    int jc_num = 0;
    u16 batteryLevel = 0;

    volatile ReportType report_type;
    volatile u32 rbuttons = 0;
    volatile u32 dbuttons = 0;
    volatile u32 buttons = 0;
    u8 packet_count = 0;
    u8 timestamp = 0;
};
#endif