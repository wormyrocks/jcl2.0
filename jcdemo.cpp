#include "libjoycon.h"
#include <unistd.h>
#ifdef _WIN32
#include "Windows.h"
#endif
#include <thread>
#include <mutex>
#include <condition_variable>

std::condition_variable exit_cv;
volatile bool signal_caught = false;

#ifdef _WIN32
BOOL WINAPI closeEvent(DWORD dwType)
{
    exit_cv.notify_all();
    return (signal_caught = true);
};
#else
void closeEvent(int s)
{
    exit_cv->notify_all();
    signal_caught = true;
    return;
};
#endif

void on_exit()
{
    jcCleanup();
}

void joyconConnected(Joycon *j)
{
    printf("Joy-Con connection callback triggered.\n");
    int ret = j->Start(JoyconSchema::SCHEMA_SIMPLE_INPUT);
    printf("Start() exited with code %d\n", ret);
    // if (ret)
    // {
    //     printf("Sleeping...");
    //     usleep(5000000);
    //     j->SetupSchema(JoyconSchema::SCHEMA_IMU);
    // }
    printf("%f\n", j->GetBatteryLevelFloat());
    printf("Rumble toggle successful? %d\n", j->ToggleParameter(ToggleParam::TP_RUMBLE, false));
    j->SetupSchema(JoyconSchema::SCHEMA_IMU);
    printf("%f\n", j->GetBatteryLevelFloat());
}

void newData(Joycon *j)
{
    printf("New data\n");
}

void newEvent(Joycon *j)
{
    printf("New input event from Joycon.\n");
}

int main(int argc, char **argv)
{
    printf("Starting, press ctrl-c to exit.\n");
#ifdef _WIN32
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)closeEvent, TRUE);
#else
    signal(SIGINT, closeEvent);
#endif
    std::mutex mtx;
    std::unique_lock<std::mutex> lck(mtx);
    registerCallback(joyconConnected, CallbackType::JOYCON_CALLBACK_CONNECTED);
    registerCallback(newData, CallbackType::JOYCON_CALLBACK_NEWDATA);
    registerCallback(newEvent, CallbackType::JOYCON_CALLBACK_NEWINPUT);
    jcBegin();
    while (!signal_caught)
    {
        exit_cv.wait(lck);
    }
    signal_caught = false;
    on_exit();
    printf("exiting\n");
}