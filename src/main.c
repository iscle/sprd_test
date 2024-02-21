#include <stdint.h>
#include "usb.h"
#include "emmc.h"

#define REG32(x) (*(volatile unsigned int *) (x))

#define UART1_BASE          0x70100000
#define UART1_TX_BUF_ADDR   (UART1_BASE)
#define UART1_TX_BUF_CNT    ((REG32(UART1_BASE + 0xC) >> 8) & 0xff)

void print_char(char c) {
    // wait until uart1 tx fifo empty
    while (UART1_TX_BUF_CNT != 0);

    // put out char by uart1 tx fifo
    REG32(UART1_TX_BUF_ADDR) = c;
}

void print_dec(uint32_t dec) {
    char buf[11];
    int i = 0;
    if (dec == 0) {
        print_char('0');
        return;
    }
    while (dec > 0) {
        buf[i++] = dec % 10 + '0';
        dec /= 10;
    }
    while (i > 0) {
        print_char(buf[--i]);
    }
}

void print_hex(uint32_t hex) {
    int i;
    char c;
    print_char('0');
    print_char('x');
    for (i = 28; i >= 0; i -= 4) {
        c = (hex >> i) & 0xf;
        if (c < 10) {
            print_char('0' + c);
        } else {
            print_char('A' + c - 10);
        }
    }
}

void print(const char *s) {
    while (*s != '\0') {
        print_char(*s++);
    }
}

extern void emmc_read();

void main() {
//    char text[] = "Hello, World!\r\n";
//    const uint8_t *receive_buffer;

    print("Calling emmc_init\r\n");
    emmc_init();
    print("emmc_init returned\r\n");
    print("Calling emmc_read\r\n");
    emmc_read();
    print("emmc_read returned\r\n");

    while (1);

//    usb_rx_init();
//
//    while (1) {
//        print(text);
//        usb_write((uint32_t *) text, sizeof(text));
//        uint16_t received = usb_read(&receive_buffer);
//        // For some reason, adding the following code crashes the entire program (not even the first "print" statement in the loop is reached)
////        if (received > 0) {
////            print("Received data\r\n");
////        } else {
////            print("No data received\r\n");
////        }
//    }
}