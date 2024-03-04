#include <stdint.h>
#include "usb.h"
#include "emmc.h"
#include "endianness.h"

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

#define VERSION_STRING "Iscle's DA 1.0.0"

__attribute__((noreturn)) void main() {
    int ret;

    usb_rx_init();
    usb_send_version(VERSION_STRING, sizeof(VERSION_STRING));

    while (1) {
        uint16_t cmd;
        uint16_t data_length;
        void *data = usb_get_data();

        ret = usb_read(&cmd, &data_length);
        if (ret) {
            print("usb_read failed\r\n");
            continue;
        }

        switch (cmd) {
            case CMD_EMMC_INIT:
                print("CMD_EMMC_INIT\r\n");

                ret = emmc_init();
                if (ret) {
                    print("emmc_init failed\r\n");
                    usb_send_status(STATUS_ERROR);
                    break;
                }

                usb_send_status(STATUS_OK);
                break;
            case CMD_EMMC_READ_SINGLE_BLOCK:
                print("CMD_EMMC_READ_SINGLE_BLOCK\r\n");

                if (data_length != 4) {
                    print("data_length != 4\r\n");
                    usb_send_status(STATUS_ERROR_DATA_LENGTH);
                    break;
                }

                uint32_t lba = READ_BE32(data);

                ret = emmc_read_single_block(lba, data);
                if (ret) {
                    print("emmc_read_single_block failed\r\n");
                    usb_send_status(STATUS_ERROR);
                    break;
                }

                usb_send_emmc_read_single_block(EMMC_BLOCK_SIZE);
                break;
            default:
                print("unknown cmd\r\n");
                break;
        }
    }
}