#include <stdint.h>

struct MODEM_TEST_TYPE {
	uint32_t receive_cnt;
	char rx_buf[64];
	uint32_t rx_index;
};
extern struct MODEM_TEST_TYPE md;

void init_modem_pins(void);
void modem_uart_init(void);

