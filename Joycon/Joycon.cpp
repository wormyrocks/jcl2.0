#include "Joycon.h"
// #include "helpers.h"
#include <hidapi.h>
#define getbyte(X, Y) u8((X >> (8 * Y)) & 0xff)

void Joycon::threadAdapter(Joycon *caller)
{
    caller->jcLoop();
}
int Joycon::hid_read_buffer(bool block)
{
    if (block)
        return hid_read_timeout(static_cast<hid_device *>(hidapi_handle), data, DATA_BUFFER_SIZE, 200);
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
    if (data[0] == report_type_names[RT_30])
    {
    }
    else if (data[0] == report_type_names[RT_21])
    {
        report_type = RT_21;
    }
    return false;
}

void Joycon::jcLoop()
{
    while (true)
    {
        if (hid_read_buffer(false) > 0 || hid_read_buffer(true) > 0)
        {
            if (callbacks[CallbackType::JOYCON_CALLBACK_NEWDATA])
                callbacks[CallbackType::JOYCON_CALLBACK_NEWDATA](this);
            datamtx->lock();
            bool eventHappened = process();
            datamtx->unlock();
            if (eventHappened && callbacks[CallbackType::JOYCON_CALLBACK_NEWDATA])
                callbacks[CallbackType::JOYCON_CALLBACK_NEWINPUT](this);
        }
        datamtx->lock();
        if (do_kill)
        {
            break;
        }
        datamtx->unlock();
        cmdmtx->lock();
        while (!fq.empty())
        {
            fq.front()();
            fq.pop_front();
        }
        cmdmtx->unlock();
    }

    // hid_close(getHidDevice());
    // printf("jcLoop killed\n");
}

Joycon::Joycon(void *handle_, JoyconType type_, int num, const char *hostmac_, std::function<void(Joycon *)> *callbacks_)
{
    callbacks = callbacks_;
    hidapi_handle = reinterpret_cast<hid_device *>(handle_);
    jtype = type_;
    jc_num = num;
    hostmac = std::string(hostmac_);
}

int Joycon::Start(JoyconSchema schema)
{
    do_kill = false;
    datamtx = new std::mutex();
    cmdmtx = new std::mutex();
    std::thread tmp(jcLoop, this);
    std::swap(tmp, jcloop);
    assert(jcloop.joinable());
    return SetupSchema(schema);
}

int Joycon::SetupSchema(JoyconSchema schema)
{
    return 1;
}

int Joycon::Stop()
{
    datamtx->lock();
    do_kill = true;
    datamtx->unlock();
    assert(jcloop.joinable());
    jcloop.join();
    return 1;
}