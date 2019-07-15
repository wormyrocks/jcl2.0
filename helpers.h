#pragma once

void threadAdapter(JoyCon *caller)
{
    caller->jcLoop();
    printf("threadAdapter done\n");
}
void JoyCon::jcSendEmpty()
{
    comm(NULL, 0, 0, 0, 0x10, 1);
}
void JoyCon::subcomm(u8 *in, u8 len, u8 subcom, u8 get_response)
{
    comm(in, len, subcom, get_response, 0x1);
}
void JoyCon::comm(u8 *in, u8 len, u8 subcom, u8 get_response, u8 command)
{
    comm(in, len, subcom, get_response, command, 0);
}
bool JoyCon::comm(u8 *in, u8 len, u8 subcom, u8 get_response, u8 command, u8 silent)
{
    // bzero(buf, len);
    buf[0] = command;
    buf[1] = packet_count;
    buf[2] = 0x0;
    buf[3] = 0x1;
    buf[4] = 0x40;
    buf[5] = 0x40;
    buf[6] = 0x0;
    buf[7] = 0x1;
    buf[8] = 0x40;
    buf[9] = 0x40;
    buf[10] = subcom;
    if (len)
    {
        for (int i = 0; i < len; ++i)
        {
            buf[11 + i] = in[i];
        }
    }
    hid_write(jc, buf, 11 + len);
    if (packet_count == 0xf)
        packet_count = 0;
    else
        ++packet_count;
    if (!get_response)
        return true;
    int attempts = 0;
    for (; attempts < SUBCOMM_ATTEMPTS_NUMBER; ++attempts)
    {
        hid_read_buffer(silent, true);
        if (data[0] != 0x21)
            continue;
        if (!(data[13] >> 7))
            continue;
        if (data[14] != subcom)
            continue;
        printf("subcomm return correct (attempt %d)\n", attempts);
        break;
    }
    return !(attempts == SUBCOMM_ATTEMPTS_NUMBER);
}

int JoyCon::hid_read_buffer(bool silent, bool block)
{
    int n;
    if (block)
        n = hid_read_timeout(jc, data, DATA_BUFFER_SIZE, 200);
    else
        n = hid_read(jc, data, DATA_BUFFER_SIZE);

    if (n > 0 && !silent)
    {
        printf("response: ");
        for (int i = 0; i < n; ++i)
        {
            printf("%02x ", data[i]);
        }
        printf("\n");
    }
    return n;
}

u8 *JoyCon::read_spi(u8 addr1, u8 addr2, int len)
{
    u8 buf[] = {addr2, addr1, 0x00, 0x00, (u8)len};
    int tries = 0;
    do
    {
        ++tries;
        subcomm(buf, 5, 0x10, 1);
    } while (tries < 10 && !(data[15] == addr2 && data[16] == addr1));
    return data + 20;
}

void JoyCon::get_stick_cal()
{
    // dump calibration data
    u8 *out = read_spi(0x80, isLeft() ? 0x12 : 0x1d, 9);
    u8 found = 0;
    for (int i = 0; i < 9; ++i)
    {
        if (out[i] != 0xff)
        {
            // User calibration data found
            std::cout << "user cal found" << std::endl;
            found = 1;
            break;
        }
    }
    if (!found)
    {
        std::cout << "User cal not found" << std::endl;
        out = read_spi(0x60, isLeft() ? 0x3d : 0x46, 9);
    }
    stick_cal[isLeft() ? 4 : 0] = ((out[7] << 8) & 0xf00) | out[6]; // X Min below center
    stick_cal[isLeft() ? 5 : 1] = ((out[8] << 4) | (out[7] >> 4));  // Y Min below center
    stick_cal[isLeft() ? 0 : 2] = ((out[1] << 8) & 0xf00) | out[0]; // X Max above center
    stick_cal[isLeft() ? 1 : 3] = ((out[2] << 4) | (out[1] >> 4));  // Y Max above center
    stick_cal[isLeft() ? 2 : 4] = ((out[4] << 8) & 0xf00 | out[3]); // X Center
    stick_cal[isLeft() ? 3 : 5] = ((out[5] << 4) | (out[4] >> 4));  // Y Center
    out = read_spi(0x60, isLeft() ? 0x86 : 0x98, 9);
    stick_cal[6] = ((out[4] << 8) & 0xF00 | out[3]); // Deadzone
}

void JoyCon::set_report_type(u8 val)
{
    subcomm(&val, 1, 0x3, 1);
}

void JoyCon::finish()
{
    // set back to normal report mode
    set_report_type(0x3f);
    // turn off LEDs
    u8 send_buf = 0;
    subcomm(&send_buf, 1, 0x30, 1);
    // turn off IMU
    toggle_parameter(TP_IMU, false);
    toggle_parameter(TP_RUMBLE, false);
}
void JoyCon::setup_joycon(u8 leds)
{
    set_report_type(0x3f);
    get_stick_cal();
    /*  TODO: improve bluetooth pairing
    send_buf = 0x1;
    subcomm(&send_buf, 1, 0x1, 1);
    send_buf = 0x2;
    subcomm(&send_buf, 1, 0x1, 1);
    send_buf = 0x3;
    subcomm(&send_buf, 1, 0x1, 1);*/
    u8 send_buf = leds;
    subcomm(&send_buf, 1, 0x30, 1);
    set_report_type(0x30);
}
void JoyCon::toggle_parameter(ToggleParam tp, bool enable_, std::condition_variable *consume)
{
    u8 sendbuf = enable_;
    subcomm(&sendbuf, 1, (u8)tp, 1);
    if (consume != NULL)
        consume->notify_all();
}
void JoyCon::set_imu_sensitivity(GyroScale gs, AccelScale as, GyroRate gr, AccelFilter af, std::condition_variable *consume)
{
    u8 sendbuf[4];
    sendbuf[0] = (u8)gs;
    sendbuf[1] = (u8)as;
    sendbuf[2] = (u8)gr;
    sendbuf[3] = (u8)af;
    subcomm(sendbuf, 4, 0x41, 1);
    if (consume != NULL)
        consume->notify_all();
}

void JoyCon::get_battery_level(std::condition_variable *consume)
{
    printf("get_battery_level\n");
    subcomm(NULL, 0, 0x50, 1);
    batteryLevel = (data[16] << 8) | data[15];
    if (consume != NULL)
        consume->notify_all();
}

void JoyCon::process()
{
    u32 buttons_ = 0;
    if (data[0] == report_type_names[RT_30])
    {
        report_type = RT_30;
        memcpy(&buttons_, (void *)(data + 3), 3);
        dbuttons = buttons_ ^ buttons;
        if (dbuttons)
        {
            rbuttons = dbuttons & ~buttons_;
            dbuttons &= buttons_;
            buttons = buttons_;
            int i = 0;
            for (; i < BUTTONS_END; ++i)
            {
                if ((rbuttons >> i) & 1)
                {
                    cout << "u";
                    cout << button_names[i] << endl;
                }
                else if ((dbuttons >> i) & 1)
                {
                    cout << "d";
                    cout << button_names[i] << endl;
                }
            }
        }

        u16 raw[] = {(uint16_t)(data[isLeft() ? 6 : 9] | ((data[isLeft() ? 7 : 10] & 0xf) << 8)),
                     (uint16_t)((data[isLeft() ? 7 : 10] >> 4) | (data[isLeft() ? 8 : 11] << 4))};

        for (u8 i = 0; i < 2; ++i)
        {
            stick[i] = (raw[i] - stick_cal[i + 2]);
            if (abs(stick[i]) < stick_cal[6])
            {
                stick[i] = 0; // inside deadzone
                continue;
            }
            else if (stick[i] > 0) // axis is above center
            {
                stick[i] /= stick_cal[i];
            }
            else // axis is below center
            {
                stick[i] /= stick_cal[i + 4];
            }
            if (stick[i] > 1)
                stick[i] = 1;
            else if (stick[i] < -1)
                stick[i] = -1;
            printf("stick: %f %f\n", stick[0], stick[1]);
        }
    }
    else if (data[0] == report_type_names[RT_21])
    {
        report_type = RT_21;
    }
}