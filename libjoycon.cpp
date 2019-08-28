#include "libjoycon.h"

void registerCallback(std::function<void(Joycon *)> callback, CallbackType cbt)
{
    callbacks[(u8)cbt] = callback;
}

void enumerateJoycons()
{
    jcBegin();
    struct hid_device_info *right_joycon_devices = hid_enumerate(NINTENDO_ID, JOYCON_R_ID);
    int i = 0;
    int allct = right_joycons.size() + left_joycons.size() + pro_cons.size();
    for (; right_joycon_devices; right_joycon_devices = right_joycon_devices->next)
    {
        if (i < right_joycons.size())
            continue;
        hid_device *hidapi_handle = hid_open_path(right_joycon_devices->path);
        hid_set_nonblocking(hidapi_handle, false);
        printf("found right Joy-Con, registering as %d\n", allct);
        Joycon *j = new Joycon(hidapi_handle, JoyconType::RIGHT, ++allct, macAddr, callbacks);
        right_joycons.push_back(j);
        if (callbacks[CallbackType::JOYCON_CALLBACK_CONNECTED])
            callbacks[CallbackType::JOYCON_CALLBACK_CONNECTED](j);
        ++i;
    }
    i = 0;
    struct hid_device_info *left_joycon_devices = hid_enumerate(NINTENDO_ID, JOYCON_L_ID);
    for (; left_joycon_devices; left_joycon_devices = left_joycon_devices->next)
    {
        if (i < left_joycons.size())
            continue;
        hid_device *hidapi_handle = hid_open_path(left_joycon_devices->path);
        hid_set_nonblocking(hidapi_handle, false);
        printf("found left Joy-Con, registering as %d\n", allct);
        Joycon *j = new Joycon(hidapi_handle, JoyconType::LEFT, ++allct, macAddr, callbacks);
        left_joycons.push_back(j);
        if (callbacks[CallbackType::JOYCON_CALLBACK_CONNECTED])
            callbacks[CallbackType::JOYCON_CALLBACK_CONNECTED](j);
        ++i;
    }
    i = 0;
    struct hid_device_info *pro_controller_devices = hid_enumerate(NINTENDO_ID, JOYCON_P_ID);
    for (; pro_controller_devices; pro_controller_devices = pro_controller_devices->next)
    {
        if (i < pro_cons.size())
            continue;
        hid_device *hidapi_handle = hid_open_path(pro_controller_devices->path);
        hid_set_nonblocking(hidapi_handle, false);
        printf("found Pro controller, registering as %d\n", allct);
        Joycon *j = new Joycon(hidapi_handle, JoyconType::PRO, ++allct, macAddr, callbacks);
        pro_cons.push_back(j);
        if (callbacks[CallbackType::JOYCON_CALLBACK_CONNECTED])
            callbacks[CallbackType::JOYCON_CALLBACK_CONNECTED](j);
        ++i;
    }
    hid_free_enumeration(left_joycon_devices);
    hid_free_enumeration(right_joycon_devices);
    hid_free_enumeration(pro_controller_devices);
}

void jcBegin()
{
    if (inited)
        return;
    hid_init();
    macAddr = "";
    inited = true;
    enumerateJoycons();
}

void jcCleanup()
{
    jcBegin();
    inited = false;
    hid_exit();
}

Joycon *getFirstJoycon()
{
    jcBegin();
    if (right_joycons.size() > 0)
        return right_joycons[0];
    if (left_joycons.size() > 0)
        return left_joycons[0];
    if (pro_cons.size() > 0)
        return pro_cons[0];
    return NULL;
}

Joycon *getJoycon(int i, JoyconType j)
{
    jcBegin();
    return NULL;
}

Joycon *waitForJoycon()
{
    jcBegin();
    return NULL;
}