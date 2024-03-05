#include "uart.h"

#define UART1_BASE      0x70100000
#define UART1_TXD       (*(volatile uint32_t *) (UART1_BASE + 0x0))
#define UART1_STS1      (*(volatile uint32_t *) (UART1_BASE + 0xC))
#define UART1_TXF_CNT   ((UART1_STS1 >> 8) & 0xFF)

void print_char(char c) {
    while (UART1_TXF_CNT != 0);
    UART1_TXD = c;
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