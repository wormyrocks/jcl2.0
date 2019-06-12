#include <Joycon.h>
#include "helpers.h"

void JoyCon::jcSetup()
{
    setup_joycon(jc, 0x1);
    printf("jcSetup\n");
}

void JoyCon::jcLoop()
{
    printf("jcLoop\n");
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
}

void JoyCon::Begin()
{
    jcSetup();
    jcLoop();
}