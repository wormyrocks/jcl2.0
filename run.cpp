#include "run.h"

// function naming conventions (will try to stick to them):
//   variables: camelcase
//   member functions with return values: camelcase
//   void functions: upper camelcase, name starts with a verb
//   type names: upper camelcase
//   c functions: lowercase, separated by underscores

void joycon_setup()
{
}

void joycon_main_loop()
{
}

int main()
{
    mac_addr = getMAC();
    std::cout << "MAC address: " << mac_addr << std::endl;
    joycon_setup();
    thread jmain(joycon_main_loop);
    jmain.join();
    return 0;
}