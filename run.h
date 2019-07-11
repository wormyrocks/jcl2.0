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
#include <assert.h>
#include <hidapi.h>
#include <unistd.h>
#include <mutex>

enum JCType:int { LEFT=0, RIGHT=1, PRO=2 };

#endif