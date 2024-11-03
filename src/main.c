#include <stdint.h>
#include <stddef.h>
#include "usb.h"
#include "emmc.h"
#include "uart.h"
#include "platform.h"

#define SOCRATES_VERSION 1

__attribute__((noreturn)) void main() {
    usb_rx_init();

    while (1) {
        int ret;
        uint16_t cmd;
        uint16_t data_length;
        void *data = USB_DATA;

        ret = usb_read(&cmd, &data_length);
        if (ret) continue;

        switch (cmd) {
            case REQ_VERSION: {
                usb_send_version(SOCRATES_VERSION);
                break;
            }
            case REQ_READ_8: {
                if (data_length != 8) {
                    usb_send_error();
                    break;
                }
                usb_send_reg8(readb((uintptr_t) READ_BE64(data)));
                break;
            }
            case REQ_WRITE_8: {
                if (data_length != 9) {
                    usb_send_error();
                    break;
                }
                writeb((uintptr_t) READ_BE64(data), READ_BE8(data + 8));
                usb_send_ok();
                break;
            }
            case REQ_READ_16: {
                if (data_length != 8) {
                    usb_send_error();
                    break;
                }
                usb_send_reg16(readw((uintptr_t) READ_BE64(data)));
                break;
            }
            case REQ_WRITE_16: {
                if (data_length != 10) {
                    usb_send_error();
                    break;
                }
                writew((uintptr_t) READ_BE64(data), READ_BE16(data + 8));
                usb_send_ok();
                break;
            }
            case REQ_READ_32: {
                if (data_length != 8) {
                    usb_send_error();
                    break;
                }
                usb_send_reg32(readl((uintptr_t) READ_BE64(data)));
                break;
            }
            case REQ_WRITE_32: {
                if (data_length != 12) {
                    usb_send_error();
                    break;
                }
                writel((uintptr_t) READ_BE64(data), READ_BE32(data + 8));
                usb_send_ok();
                break;
            }
            case REQ_READ_64: {
                if (data_length != 8) {
                    usb_send_error();
                    break;
                }
                usb_send_reg64(readq((uintptr_t) READ_BE64(data)));
                break;
            }
            case REQ_WRITE_64: {
                if (data_length != 16) {
                    usb_send_error();
                    break;
                }
                writeq((uintptr_t) READ_BE64(data), READ_BE64(data + 8));
                usb_send_ok();
                break;
            }
        }

        // switch (cmd) {
        //     case CMD_EMMC_INIT: {
        //         print("CMD_EMMC_INIT\r\n");
        //         ret = emmc_init();
        //         if (ret) {
        //             print("CMD_EMMC_INIT: emmc_init failed\r\n");
        //             usb_send_status(STATUS_ERROR);
        //             break;
        //         }

        //         usb_send_status(STATUS_OK);
        //         break;
        //     }
        //     case CMD_EMMC_SWITCH: {
        //         print("CMD_EMMC_SWITCH\r\n");
        //         if (data_length != 2) {
        //             print("CMD_EMMC_SWITCH: data_length != 2\r\n");
        //             usb_send_status(STATUS_ERROR_DATA_LENGTH);
        //             break;
        //         }

        //         uint8_t index = ((uint8_t *) data)[0];
        //         uint8_t value = ((uint8_t *) data)[1];

        //         ret = emmc_switch(index, value, NULL);
        //         if (ret) {
        //             print("CMD_EMMC_SWITCH: emmc_switch failed\r\n");
        //             usb_send_status(STATUS_ERROR);
        //             break;
        //         }

        //         usb_send_status(STATUS_OK);
        //         break;
        //     }
        //     case CMD_EMMC_GET_SEC_COUNT: {
        //         print("CMD_EMMC_GET_SEC_COUNT\r\n");
        //         uint32_t sec_count = emmc_get_sec_count();
        //         usb_send_emmc_sec_count(sec_count);
        //         break;
        //     }
        //     case CMD_EMMC_READ_SINGLE_BLOCK: {
        //         print("CMD_EMMC_READ_SINGLE_BLOCK\r\n");
        //         if (data_length != 4) {
        //             print("CMD_EMMC_READ_SINGLE_BLOCK: data_length != 4\r\n");
        //             usb_send_status(STATUS_ERROR_DATA_LENGTH);
        //             break;
        //         }

        //         uint32_t lba = READ_BE32(data);

        //         ret = emmc_read_single_block(lba, data);
        //         if (ret) {
        //             print("CMD_EMMC_READ_SINGLE_BLOCK: emmc_read_single_block failed\r\n");
        //             usb_send_status(STATUS_ERROR);
        //             break;
        //         }

        //         usb_send_emmc_read_single_block(EMMC_BLOCK_SIZE);
        //         break;
        //     }
        //     case CMD_EMMC_WRITE_BLOCK: {
        //         print("CMD_EMMC_WRITE_BLOCK\r\n");
        //         if (data_length != 4 + EMMC_BLOCK_SIZE) {
        //             print("CMD_EMMC_WRITE_BLOCK: data_length != 4 + EMMC_BLOCK_SIZE\r\n");
        //             usb_send_status(STATUS_ERROR_DATA_LENGTH);
        //             break;
        //         }

        //         uint32_t lba = READ_BE32(data);

        //         ret = emmc_write_block(lba, data + 4);
        //         if (ret) {
        //             print("CMD_EMMC_WRITE_BLOCK: emmc_write_block failed\r\n");
        //             usb_send_status(STATUS_ERROR);
        //             break;
        //         }

        //         usb_send_status(STATUS_OK);
        //         break;
        //     }
        // }
    }
}