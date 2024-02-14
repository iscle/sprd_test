#include "usb.h"

#define REG32(x) (*(volatile unsigned int *) (x))

#define UART1_BASE			0x70100000
#define UART1_TX_BUF_ADDR	(UART1_BASE)
#define UART1_TX_BUF_CNT	((REG32(UART1_BASE + 0xC) >> 8) & 0xff)

void print(const char *s) {
    while (*s != '\0') {
        // wait until uart1 tx fifo empty
        while (UART1_TX_BUF_CNT != 0);

        // put out char by uart1 tx fifo
        REG32(UART1_TX_BUF_ADDR) = *s;
        s++;
    }
}

void main() {
    char text[] = "Hello, World!\r\n";
    const unsigned char *receive_buffer;

    usb_rx_init();

	while (1) {
		print(text);
        usb_write((unsigned int *) text, sizeof(text));
        unsigned short received = usb_read(&receive_buffer);
        // For some reason, adding the following code crashes the entire program (not even the first "print" statement in the loop is reached)
//        if (received > 0) {
//            print("Received data\r\n");
//        } else {
//            print("No data received\r\n");
//        }
	}
}