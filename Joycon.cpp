#include <Joycon.h>
#include "helpers.h"
#include <thread>
#include <mutex>

void JoyCon::jcSetup()
{
    setup_joycon(jc, 0x1);
    printf("jcSetup\n");
}

void JoyCon::jcLoop()
{
    while (!do_kill){
	    jcSendEmpty();
    }
}
bool JoyCon::isConnected() { return 0; };

JoyCon::JoyCon(hid_device *handle_, JCType type_, char *hostmac_)
{
    jc = handle_;
    jtype = type_;
    hostmac = string(hostmac_);
}

void JoyCon::Cleanup()
{
    do_kill = true;
    jcloop.join();
    hid_close(getHidDevice());
}

void JoyCon::Begin()
{
    jcSetup();
    thread tmp(threadAdapter,(void*)(this));
    swap(tmp, jcloop);
}
