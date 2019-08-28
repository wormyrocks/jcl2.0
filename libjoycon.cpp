#include "libjoycon.h"

void registerConnectionCallback(std::function<void(Joycon *)> callback)
{
    connectionCallback = callback;
}

void enumerateJoycons()
{
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
        Joycon *j = new Joycon(hidapi_handle, JOYCON_TYPE::RIGHT, ++allct, macAddr);
        right_joycons.push_back(j);
        if (connectionCallback)
            connectionCallback(j);
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
        Joycon *j = new Joycon(hidapi_handle, JOYCON_TYPE::LEFT, ++allct, macAddr);
        left_joycons.push_back(j);
        if (connectionCallback)
            connectionCallback(j);
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
        Joycon *j = new Joycon(hidapi_handle, JOYCON_TYPE::PRO, ++allct, macAddr);
        pro_cons.push_back(j);
        if (connectionCallback)
            connectionCallback(j);
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
    inited = false;
    hid_exit();
}

Joycon *getFirstJoycon()
{
    if (right_joycons.size() > 0)
        return right_joycons[0];
    if (left_joycons.size() > 0)
        return left_joycons[0];
    if (pro_cons.size() > 0)
        return pro_cons[0];
    return NULL;
}

Joycon *getJoycon(int i, JOYCON_TYPE j)
{
    return NULL;
}

Joycon *waitForJoycon()
{
    return NULL;
}