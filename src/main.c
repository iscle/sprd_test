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

#define VERSION_STRING "Iscle's DA 1.0.0"

void main() {
    int ret;

    usb_rx_init();
    usb_send_version(VERSION_STRING, sizeof(VERSION_STRING));

    while (1) {
        usb_cmd_t cmd;
        uint16_t data_length;
        void *data;

        ret = usb_read(&cmd, &data_length, &data);
        if (ret) {
            print("usb_read failed\r\n");
            continue;
        }

        switch (cmd) {
            case CMD_EMMC_INIT:
                print("CMD_EMMC_INIT\r\n");
                ret = emmc_init();
                print("emmc_init ret: ");
                print_dec(ret);
                print("\r\n");
                if (ret) {
                    usb_send_status(STATUS_ERROR);
                } else {
                    usb_send_status(STATUS_OK);
                }
                break;
            case CMD_EMMC_READ:
                print("CMD_EMMC_READ\r\n");
                emmc_read();
                usb_send_status(STATUS_OK);
                break;
            default:
                print("unknown cmd\r\n");
                break;
        }
    }
}