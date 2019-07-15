#include <Joycon.h>
#include "helpers.h"
#include <thread>
#include <mutex>

void JoyCon::jcSetup()
{
    printf("jcSetup\n");
    setup_joycon((u8)jc_num);
}

void JoyCon::jcLoop()
{
    jcSetup();
    printf("setup done, main loop start (ctrl-c to exit)\n");
    bool show_output = false;
    while (true)
    {
        if (hid_read_buffer(!show_output, false) > 0 || hid_read_buffer(!show_output, true) > 0)
        {
            datamtx->lock();
            process();
            datamtx->unlock();
        }
        cmdmtx->lock();
        datamtx->lock();
        if (do_kill)
        {
            break;
        }
        datamtx->unlock();
        while (!fq.empty())
        {
            fq.front()();
            fq.pop_front();
        }
        cmdmtx->unlock();
    }
    finish();
    hid_close(getHidDevice());
    printf("jcLoop killed\n");
}

bool JoyCon::isConnected() { return 0; };

JoyCon::JoyCon(hid_device *handle_, JCType type_, int num, char *hostmac_)
{
    jc = handle_;
    jtype = type_;
    jc_num = num;
    hostmac = string(hostmac_);
    do_kill = false;
    datamtx = new mutex();
    cmdmtx = new mutex();
    thread tmp(threadAdapter, this);
    swap(tmp, jcloop);
    assert(jcloop.joinable());
}

// public function
void JoyCon::ToggleParameter(ToggleParam tp, bool enable)
{
    assert(tp < TP_END);
    std::mutex mtx;
    std::unique_lock<std::mutex> lck(mtx);
    std::condition_variable consume;
    cmdmtx->lock();
    printf("Toggling param: %02x to value %d\n", tp, enable);
    fq.push_back([this, tp, enable, &consume] { this->toggle_parameter(tp, enable, &consume); });
    printf("Function queued\n");
    cmdmtx->unlock();
    consume.wait_for(lck, std::chrono::milliseconds(100));
}

u16 JoyCon::GetBatteryLevel()
{
    std::mutex mtx;
    std::unique_lock<std::mutex> lck(mtx);
    std::condition_variable consume;
    cmdmtx->lock();
    printf("cmdmtx lock acquired on main thread\n");
    fq.push_back([this, &consume] { this->get_battery_level(&consume); });
    printf("Function queued\n");
    cmdmtx->unlock();
    if (consume.wait_for(lck, std::chrono::milliseconds(100)) == std::cv_status::timeout)
    {
        return 0;
    }
    return batteryLevel;
}

float JoyCon::GetBatteryLevelFloat()
{
    return (float)(GetBatteryLevel() - 1320) / 360;
}

void JoyCon::Cleanup()
{
    datamtx->lock();
    do_kill = true;
    datamtx->unlock();
    printf("kill flag set\n");
    assert(jcloop.joinable());
    jcloop.join();
}