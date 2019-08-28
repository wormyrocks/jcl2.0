#ifndef LIBJOYCON_H
#define LIBJOYCON_H
#ifdef BUILDING_SHARED_LIB
#include <hidapi.h>
#endif
#include "Joycon/Joycon.h"

std::vector<JoyConObj *> left_joycons;
std::vector<JoyConObj *> right_joycons;
std::vector<JoyConObj *> pro_cons;
std::function<void(JoyCon *)> connectionCallback = NULL;
char const *macAddr;
bool inited = false;

EXPORT_DECL void jcBegin();
EXPORT_DECL void jcCleanup();
EXPORT_DECL JoyCon *getFirstJoycon();
EXPORT_DECL JoyCon *getJoycon(int i, JOYCON_TYPE j);
EXPORT_DECL JoyCon *waitForJoycon();
EXPORT_DECL void registerConnectionCallback(std::function<void(JoyCon *)> callback);
#endif