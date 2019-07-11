#include <Joycon.h>
#include "helpers.h"
#include <thread>
#include <mutex>

void JoyCon::jcSetup()
{
    printf("jcSetup\n");
    setup_joycon(jc, (u8)jc_num);
}

void JoyCon::jcLoop()
{
    jcSetup();
    printf("setup done, main loop start (ctrl-c to exit)\n");
    while (true)
    {
        jcSendEmpty();
        killmtx->lock();
        if (do_kill)
        {
            break;
        }
        killmtx->unlock();
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
    killmtx = new mutex();
    thread tmp(threadAdapter, this);
    swap(tmp, jcloop);
    assert(jcloop.joinable());
}

void JoyCon::Cleanup()
{
    killmtx->lock();
    do_kill = true;
    killmtx->unlock();
    printf("kill flag set\n");
    assert(jcloop.joinable());
    jcloop.join();
}