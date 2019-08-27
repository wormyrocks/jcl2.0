#include <run.h>
#include <Joycon.h>
#include "grabmac.h"
#include <vector>

vector<JoyCon *> left_joycons;
vector<JoyCon *> right_joycons;
vector<JoyCon *> pro_cons;
char *macAddr = "";

// function naming conventions (will try to stick to them):
//   variables: camelcase
//   member functions with return values: camelcase
//   void functions: upper camelcase, name starts with a verb
//   type names: upper camelcase
//   c functions: lowercase, separated by underscores

volatile bool signal_caught = false;

#ifdef _WIN32
BOOL WINAPI closeEvent(DWORD dwType)
{
    return (signal_caught = true);
};
#else
void closeEvent(int s)
{
    signal_caught = true;
    return;
};
#endif

JoyCon *FirstJoycon()
{
    if (right_joycons.size() > 0)
        return right_joycons[0];
    if (left_joycons.size() > 0)
        return left_joycons[0];
    if (pro_cons.size() > 0)
        return pro_cons[0];
    return NULL;
}

void on_sigint()
{
    for (int i = 0; i < right_joycons.size(); ++i)
    {
        printf("cleaning up rcon %d\n", i);
        right_joycons[i]->Cleanup();
    }
    for (int i = 0; i < left_joycons.size(); ++i)
    {
        printf("cleaning up lcon %d\n", i);
        left_joycons[i]->Cleanup();
    }
    for (int i = 0; i < pro_cons.size(); ++i)
    {
        printf("cleaning up pcon %d\n", i);
        pro_cons[i]->Cleanup();
    }
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
        std::cout << "found right Joy-Con, registering as " << allct << std::endl;
        right_joycons.push_back(new JoyCon(hidapi_handle, JCType::RIGHT, ++allct, macAddr));
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
        std::cout << "found left Joy-Con, registering as " << allct << std::endl;
        left_joycons.push_back(new JoyCon(hidapi_handle, JCType::LEFT, ++allct, macAddr));
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
        std::cout << "found Pro Controller, registering as " << allct << std::endl;
        pro_cons.push_back(new JoyCon(hidapi_handle, JCType::PRO, ++allct, macAddr));
        ++i;
    }
    hid_free_enumeration(left_joycon_devices);
    hid_free_enumeration(right_joycon_devices);
    hid_free_enumeration(pro_controller_devices);
}
int main()
{
#ifdef _WIN32
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)closeEvent, TRUE);
#else
    signal(SIGINT, closeEvent);
#endif
    printf("Waiting for connections.\n");
    hid_init();
    /*    *macAddr = getMAC();
    std::cout << "MAC address: " << macAddr << std::endl;*/
    bool done = false;
    long i = 0;
    while (!done)
    {
        enumerateJoycons();
        usleep(500000); // TODO: change to CV
        if (signal_caught)
        {
            signal_caught = false;
            on_sigint();
            printf("on_sigint finished\n");
            done = true;
        }
        if (i % 2 == 0)
        {
            JoyCon *j = FirstJoycon();
            if (j)
            {
                float *acc_g = j->GetRawAccel();
                float *gyr_dps = j->GetRawGyro();
                printf("accel data: [%f %f %f], gyro data: ", acc_g[0], acc_g[1], acc_g[2]);
                printf("[%f %f %f]\n", gyr_dps[0], gyr_dps[1], gyr_dps[2]);
                // printf("getBatteryLevel called\n");
                // // float f = right_joycons[0]->GetBatteryLevelFloat();
                // right_joycons[0]->ToggleParameter(JoyCon::TP_IMU, true);
                // printf("toggleIMU finished\n");
            }
        }
        ++i;
    }
    hid_exit();
    return 0;
}
