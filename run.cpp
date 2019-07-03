#include <run.h>
#include <Joycon.h>
#include "grabmac.h"
#include <vector>

vector<JoyCon> left_joycons;
vector<JoyCon> right_joycons;
vector<JoyCon> pro_cons;

// function naming conventions (will try to stick to them):
//   variables: camelcase
//   member functions with return values: camelcase
//   void functions: upper camelcase, name starts with a verb
//   type names: upper camelcase
//   c functions: lowercase, separated by underscores

int main()
{
/*    char *mac_addr = getMAC();
    std::cout << "MAC address: " << mac_addr << std::endl;*/
	char *mac_addr = "";
    hid_init();
    struct hid_device_info *right_joycon_devices = hid_enumerate(NINTENDO_ID, JOYCON_R_ID);
    int i = 0;
    for (; right_joycon_devices; right_joycon_devices = right_joycon_devices->next)
    {
        if (hid_device *hidapi_handle = hid_open_path(right_joycon_devices->path))
        {
            right_joycons.push_back(JoyCon(hidapi_handle, JCType::RIGHT, mac_addr));
            ++i;
        }
    }
    if (i > 0)
        std::cout << "found " << i << " right Joy-Con" << std::endl;
    i = 0;
    struct hid_device_info *left_joycon_devices = hid_enumerate(NINTENDO_ID, JOYCON_L_ID);
    for (; left_joycon_devices; left_joycon_devices = left_joycon_devices->next)
    {
        hid_device *hidapi_handle = hid_open_path(left_joycon_devices->path);
        left_joycons.push_back(JoyCon(hidapi_handle, JCType::LEFT, mac_addr));
        ++i;
    }
    if (i > 0)
        std::cout << "found " << i << " right Joy-Con" << std::endl;
    i = 0;
    struct hid_device_info *pro_controller_devices = hid_enumerate(NINTENDO_ID, JOYCON_P_ID);
    for (; pro_controller_devices; pro_controller_devices = pro_controller_devices->next)
    {
        hid_device *hidapi_handle = hid_open_path(pro_controller_devices->path);
        pro_cons.push_back(JoyCon(hidapi_handle, JCType::PRO, mac_addr));
        ++i;
    }
    if (i > 0)
        std::cout << "found " << i << " Pro Controller" << std::endl;
    for (int i = 0; i < right_joycons.size(); ++i)
    {
	right_joycons[i].Begin();
    }
    for (int i = 0; i < left_joycons.size(); ++i)
    {
	left_joycons[i].Begin();
    }
    for (int i = 0; i < pro_cons.size(); ++i)
    {
	pro_cons[i].Begin();
    }
    int ch = std::cin.get();
    for (int i = 0; i < right_joycons.size(); ++i)
    {
	right_joycons[i].Cleanup();
    }
    for (int i = 0; i < left_joycons.size(); ++i)
    {
	left_joycons[i].Cleanup();
    }
    for (int i = 0; i < pro_cons.size(); ++i)
    {
	pro_cons[i].Cleanup();
    }
    return 0;
}
