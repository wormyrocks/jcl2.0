#ifndef LIBJOYCON_H
#define LIBJOYCON_H
#ifdef BUILDING_SHARED_LIB
#include <hidapi.h>
#endif
#include "Joycon/Joycon.h"

std::vector<Joycon *> left_joycons;
std::vector<Joycon *> right_joycons;
std::vector<Joycon *> pro_cons;
std::function<void(Joycon *)> callbacks[CallbackType::JOYCON_CALLBACK_LENGTH] = {nullptr};
char const *macAddr;
bool inited = false;

EXPORT_DECL void jcBegin();
EXPORT_DECL void jcCleanup();
EXPORT_DECL Joycon *getFirstJoycon();
EXPORT_DECL Joycon *getJoycon(int i, JoyconType j);
EXPORT_DECL Joycon *waitForJoycon();
EXPORT_DECL void registerCallback(std::function<void(Joycon *)> callback, CallbackType cbt);
#endif