#include "Joycon.h"
// #include "helpers.h"
#include <hidapi.h>
static void jcLoop(Joycon *caller)
{
    // jcSetup();
    // bool show_output = false;
    // while (true)
    // {
    //     if (hid_read_buffer(!show_output, false) > 0 || hid_read_buffer(!show_output, true) > 0)
    //     {
    //         datamtx->lock();
    //         process();
    //         datamtx->unlock();
    //     }
    //     datamtx->lock();
    //     if (do_kill)
    //     {
    //         break;
    //     }
    //     datamtx->unlock();
    //     cmdmtx->lock();
    //     while (!fq.empty())
    //     {
    //         fq.front()();
    //         fq.pop_front();
    //     }
    //     cmdmtx->unlock();
    // }
    // finish();
    // hid_close(getHidDevice());
    // printf("jcLoop killed\n");
}

Joycon::Joycon(void *handle_, JOYCON_TYPE type_, int num, const char *hostmac_)
{
    hidapi_handle = reinterpret_cast<hid_device *>(handle_);
    jtype = type_;
    jc_num = num;
    hostmac = std::string(hostmac_);
}

int Joycon::Start(JOYCON_SCHEMA schema, bool rumble_enabled)
{
    do_kill = false;
    datamtx = new std::mutex();
    cmdmtx = new std::mutex();
    std::thread tmp(jcLoop, this);
    std::swap(tmp, jcloop);
    assert(jcloop.joinable());
    return 1;
}
