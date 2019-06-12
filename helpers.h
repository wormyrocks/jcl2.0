#pragma once

void subcomm(hid_device* joycon, u8* in, u8 len, u8 comm, u8 get_response, u8 is_left)
{
	u8 buf[OUT_BUFFER_SIZE] = { 0 };
	buf[0] = 0x1;
	buf[1] = global_counter[is_left];
	buf[2] = 0x0;
	buf[3] = 0x1;
	buf[4] = 0x40;
	buf[5] = 0x40;
	buf[6] = 0x0;
	buf[7] = 0x1;
	buf[8] = 0x40;
	buf[9] = 0x40;
	buf[10] = comm;
	for (int i = 0; i < len; ++i) {
		buf[11 + i] = in[i];
	}
	if (is_left) {
		if (global_counter[is_left] == 0xf) global_counter[is_left] = 0;
		else ++global_counter[is_left];
	}
	else {
		if (global_counter[is_left] == 0xf) global_counter[is_left] = 0;
		else ++global_counter[is_left];
	}
	//for (int i = 0; i < 15; ++i) {
	//	printf("%x ", buf[i]);
	//}
	//printf("\n");
	hid_write(joycon, buf, OUT_BUFFER_SIZE);
	if (get_response) {
		int n = hid_read_timeout(joycon, data, DATA_BUFFER_SIZE, 50);

		/*printf("response: ");
		for (int i = 0; i < 35; ++i) {
			printf("%x ", data[i]);
		}
		printf("\n");

		if (data[14] != comm) {
			printf("subcomm return fail\n");
		}
		else printf("subcomm return correct\n");
		*/
	}
}

u8* read_spi(hid_device *jc, u8 addr1, u8 addr2, int len, u8 is_left)
{
	u8 buf[] = { addr2, addr1, 0x00, 0x00, (u8)len };
	int tries = 0;
	do {
		++tries;
		subcomm(jc, buf, 5, 0x10, 1, is_left);
	} while (tries < 10 && !(data[15] == addr2 && data[16] == addr1));
	return data + 20;
}

void get_stick_cal(hid_device* jc, u8 is_left)
{
	// dump calibration data
	u8* out = read_spi(jc, 0x80, is_left ? 0x12 : 0x1d, 9, is_left);
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
		out = read_spi(jc, 0x60, is_left ? 0x3d : 0x46, 9, is_left);
	}
	stick_cal[is_left ? 4 : 7]  = ((out[7] << 8) & 0xf00) | out[6]; // X Min below center
	stick_cal[is_left ? 5 : 8]  = ((out[8] << 4) | (out[7] >> 4));  // Y Min below center
	stick_cal[is_left ? 0 : 9]  = ((out[1] << 8) & 0xf00) | out[0]; // X Max above center
	stick_cal[is_left ? 1 : 10] = ((out[2] << 4) | (out[1] >> 4));  // Y Max above center
	stick_cal[is_left ? 2 : 11] = ((out[4] << 8) & 0xf00 | out[3]); // X Center
	stick_cal[is_left ? 3 : 12] = ((out[5] << 4) | (out[4] >> 4));  // Y Center
	out = read_spi(jc, 0x60, is_left ? 0x86 : 0x98, 9, is_left);
	stick_cal[is_left ? 6 : 13] = ((out[4] << 8) & 0xF00 | out[3]);	// Deadzone
}

void setup_joycon(hid_device *jc, u8 leds, u8 is_left) {
	u8 send_buf = 0x3f;
	subcomm(jc, &send_buf, 1, 0x3, 1, is_left);
	get_stick_cal(jc, is_left);	
/*	TODO: improve bluetooth pairing
	send_buf = 0x1;
	subcomm(jc, &send_buf, 1, 0x1, 1, is_left);
	send_buf = 0x2;
	subcomm(jc, &send_buf, 1, 0x1, 1, is_left);
	send_buf = 0x3;
	subcomm(jc, &send_buf, 1, 0x1, 1, is_left);*/
	send_buf = leds;
	subcomm(jc, &send_buf, 1, 0x30, 1, is_left);
	send_buf = 0x30;
	subcomm(jc, &send_buf, 1, 0x3, 1, is_left);
}