#ifndef _RUN
#define _RUN

using namespace std;

#ifdef _WIN32
#include "Windows.h"
#endif

#include <string>
#include <iostream>
#include <thread>
#include <csignal>
#include <hidapi.h>

enum JCType:int { LEFT=0, RIGHT=1, PRO=2 };

#endif