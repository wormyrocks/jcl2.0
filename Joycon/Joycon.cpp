#include "Joycon.h"
// #include "helpers.h"
#include <hidapi.h>
#define getbyte(X, Y) u8((X >> (8 * Y)) & 0xff)

void Joycon::threadAdapter(Joycon *caller)
{
    caller->jcLoop();
}
int Joycon::hid_read_buffer(bool nonblock)
{
    if (nonblock)
        return hid_read_timeout(static_cast<hid_device *>(hidapi_handle), data, DATA_BUFFER_SIZE, 100);
    return hid_read(static_cast<hid_device *>(hidapi_handle), data, DATA_BUFFER_SIZE);
}
u8 *Joycon::read_spi(u32 addr, int len)
{
    u8 send_buf[] = {getbyte(addr, 0), getbyte(addr, 1), getbyte(addr, 2), getbyte(addr, 3), (u8)len};
    int tries = 0;
    do
    {
        ++tries;
        subcomm(send_buf, 5, SC_SPI_FLASH_READ, 1);
    } while (tries < 10 && !(data[15] == send_buf[0] && data[16] == send_buf[1] && data[17] == send_buf[2] && data[18] == send_buf[3]));
    return data + 20;
}
bool Joycon::subcomm(u8 *in, u8 len, SubcommandType subcom, bool get_response)
{
    return comm(in, len, subcom, get_response, 0x1);
}
bool Joycon::comm(u8 *in, u8 len, SubcommandType subcom, bool get_response, u8 command)
{
    // bzero(buf, len);
    outbuf[0] = command;
    outbuf[1] = packet_count;
    outbuf[2] = 0x0;
    outbuf[3] = 0x1;
    outbuf[4] = 0x40;
    outbuf[5] = 0x40;
    outbuf[6] = 0x0;
    outbuf[7] = 0x1;
    outbuf[8] = 0x40;
    outbuf[9] = 0x40;
    outbuf[10] = subcom;
    if (len)
    {
        for (int i = 0; i < len; ++i)
        {
            outbuf[11 + i] = in[i];
        }
    }
    hid_write(static_cast<hid_device *>(hidapi_handle), outbuf, 11 + len);
    if (packet_count == 0xf)
        packet_count = 0;
    else
        ++packet_count;
    if (!get_response)
        return true;
    int attempts = 0;
    for (; attempts < SUBCOMM_ATTEMPTS_NUMBER; ++attempts)
    {
        hid_read_buffer(true);
        if (data[0] != 0x21)
            continue;
        if (!(data[13] >> 7))
            continue;
        if (data[14] != subcom)
            continue;
        break;
    }
    return !(attempts == SUBCOMM_ATTEMPTS_NUMBER);
}

bool Joycon::process()
{
    u32 buttons_ = 0;
    // if (data[0] == report_type_names[RT_30])
    // {
    // }
    // else if (data[0] == report_type_names[RT_21])
    // {
    //     report_type = RT_21;
    // }
    return false;
}

void Joycon::jcLoop()
{
    while (true)
    {
        if (hid_read_buffer(true) > 0)
        {
            // TODO : should a long callback block this update loop? probably
            if (callbacks[CallbackType::JOYCON_CALLBACK_NEWDATA])
                callbacks[CallbackType::JOYCON_CALLBACK_NEWDATA](this);
            datamtx->lock();
            bool eventHappened = process();
            datamtx->unlock();
            if (eventHappened && callbacks[CallbackType::JOYCON_CALLBACK_NEWINPUT])
                callbacks[CallbackType::JOYCON_CALLBACK_NEWINPUT](this);
        }
        killmtx->lock();
        if (do_kill)
            break;
        killmtx->unlock();
        if (new_command)
        {
            cmdmtx->lock();
            hid_set_nonblocking(static_cast<hid_device *>(hidapi_handle), false);
            new_command = false;
            while (!fq.empty())
            {
                fq.front()();
                fq.pop_front();
            }
            cmdmtx->unlock();
        }
    }
    hid_close(static_cast<hid_device *>(hidapi_handle));
}

Joycon::Joycon(void *handle_, JoyconType type_, int num, const char *hostmac_, std::function<void(Joycon *)> *callbacks_)
{
    callbacks = callbacks_;
    hidapi_handle = static_cast<hid_device *>(handle_);
    jtype = type_;
    jc_num = num;
    hostmac = std::string(hostmac_);
}

bool Joycon::Start(JoyconSchema schema)
{
    do_kill = false;
    datamtx = new std::mutex();
    killmtx = new std::mutex();
    cmdmtx = new std::mutex();
    std::thread tmp(jcLoop, this);
    std::swap(tmp, jcloop);
    assert(jcloop.joinable());
    if (SetupSchema(schema))
        started = true;
    return started;
}

ReportType Joycon::SetReportType(ReportType val, std::condition_variable *consume)
{
    report_type = val;
    subcomm((u8 *)&val, 1, SC_SET_INPUT_REPORT_MODE, 1);
    if (consume != NULL)
        consume->notify_all();
    return val;
}

void Joycon::Unblock()
{
    hid_set_nonblocking(static_cast<hid_device *>(hidapi_handle), true);
    new_command = true;
}

ReportType Joycon::SetReportType(ReportType val)
{
    std::mutex mtx;
    std::unique_lock<std::mutex> lck(mtx);
    std::condition_variable consume;
    cmdmtx->lock();
    fq.push_back([this, val, &consume] { this->SetReportType(val, &consume); });
    Unblock();
    cmdmtx->unlock();
    if (consume.wait_for(lck, std::chrono::milliseconds(CV_TIMEOUT)) == std::cv_status::timeout)
        return RT_ERROR;
    return val;
}

int Joycon::SetupSchema(JoyconSchema schema, std::condition_variable *consume)
{
    switch (schema)
    {
    case SCHEMA_NOCONFIG:
        break;
    case SCHEMA_CAMERA:
        // TODO
        break;
    case SCHEMA_SIMPLE_INPUT:
        SetReportType(RT_SIMPLEHID, NULL);
        break;
    case SCHEMA_GAMEPAD:
        SetReportType(RT_FULL, NULL);
        break;
    case SCHEMA_IMU:
        SetReportType(RT_FULL, NULL);
        ToggleParameter(TP_IMU, true, NULL);
        break;
    }
    if (consume != NULL)
        consume->notify_all();
    return 1;
}

void Joycon::SendEmpty()
{
    comm(NULL, 0, SC_NOTHING, 0, 0x10);
}

int Joycon::SetupSchema(JoyconSchema schema)
{
    printf("Switching to schema %d...");
    if (schema == SCHEMA_NOCONFIG)
        return 1;
    std::mutex mtx;
    std::unique_lock<std::mutex> lck(mtx);
    std::condition_variable consume;
    cmdmtx->lock();
    fq.push_back([this, schema, &consume] { this->SetupSchema(schema, &consume); });
    Unblock();
    cmdmtx->unlock();
    if (consume.wait_for(lck, std::chrono::milliseconds(CV_TIMEOUT)) == std::cv_status::timeout)
        return 0;
    return 1;
}

int Joycon::ToggleParameter(ToggleParam tp, bool enable)
{
    std::mutex mtx;
    std::unique_lock<std::mutex> lck(mtx);
    std::condition_variable consume;
    cmdmtx->lock();
    fq.push_back([this, tp, enable, &consume] { this->ToggleParameter(tp, enable, &consume); });
    Unblock();
    cmdmtx->unlock();
    if (consume.wait_for(lck, std::chrono::milliseconds(CV_TIMEOUT)) == std::cv_status::timeout)
        return 0;
    return 1;
}

int Joycon::ToggleParameter(ToggleParam tp, bool enable, std::condition_variable *consume)
{
    u8 sendbuf = enable;
    subcomm(&sendbuf, 1, (SubcommandType)tp, 1);
    if (consume != NULL)
        consume->notify_all();
    return (int)enable;
}

u16 Joycon::GetBatteryLevel(std::condition_variable *consume)
{
    subcomm(NULL, 0, SC_GET_VOLTAGE, 1);
    batteryLevel = (data[16] << 8) | data[15];
    if (consume != NULL)
        consume->notify_all();
    return batteryLevel;
}

u16 Joycon::GetBatteryLevel()
{
    std::mutex mtx;
    std::unique_lock<std::mutex> lck(mtx);
    std::condition_variable consume;
    cmdmtx->lock();
    fq.push_back([this, &consume] { this->GetBatteryLevel(&consume); });
    Unblock();
    cmdmtx->unlock();
    if (consume.wait_for(lck, std::chrono::milliseconds(CV_TIMEOUT)) == std::cv_status::timeout)
        return 0;
    return batteryLevel;
}

float Joycon::GetBatteryLevelFloat()
{
    return (float)(GetBatteryLevel() - 1320) / 360;
}

bool Joycon::Stop()
{
    killmtx->lock();
    do_kill = true;
    killmtx->unlock();
    assert(jcloop.joinable());
    jcloop.join();
    return 1;
}