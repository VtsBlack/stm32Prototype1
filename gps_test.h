#include <stdint.h>

struct GPS_TEST_TYPE {
	uint32_t receive_cnt;
	char rx_buf[64];
	uint32_t rx_index;
};
extern struct GPS_TEST_TYPE gp;

void init_gps_pins(void);
void init_gps_uart(void);

