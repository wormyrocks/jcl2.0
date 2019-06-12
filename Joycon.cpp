#include <Joycon.h>

void JoyCon::joycon_main_loop()
{
    printf("hi\n");
}
bool JoyCon::isConnected() { return 0; };

JoyCon::JoyCon(hid_device *handle_, JCType type_)
{
    jc = handle_;
    type = type_;
}