#include <stdint.h>
#include <stddef.h>
#include "usb.h"
#include "emmc.h"
#include "endianness.h"
#include "uart.h"

#define VERSION_STRING "Iscle's DA 1.0.0"

__attribute__((noreturn)) void main() {
    usb_rx_init();
    usb_send_version(VERSION_STRING, sizeof(VERSION_STRING));

    while (1) {
        int ret;
        uint16_t cmd;
        uint16_t data_length;
        void *data = usb_get_data();

        ret = usb_read(&cmd, &data_length);
        if (ret) {
            continue;
        }

        switch (cmd) {
            case CMD_EMMC_INIT: {
                print("CMD_EMMC_INIT\r\n");
                ret = emmc_init();
                if (ret) {
                    print("CMD_EMMC_INIT: emmc_init failed\r\n");
                    usb_send_status(STATUS_ERROR);
                    break;
                }

                usb_send_status(STATUS_OK);
                break;
            }
            case CMD_EMMC_SWITCH: {
                print("CMD_EMMC_SWITCH\r\n");
                if (data_length != 2) {
                    print("CMD_EMMC_SWITCH: data_length != 2\r\n");
                    usb_send_status(STATUS_ERROR_DATA_LENGTH);
                    break;
                }

                uint8_t index = ((uint8_t *) data)[0];
                uint8_t value = ((uint8_t *) data)[1];

                ret = emmc_switch(index, value, NULL);
                if (ret) {
                    print("CMD_EMMC_SWITCH: emmc_switch failed\r\n");
                    usb_send_status(STATUS_ERROR);
                    break;
                }

                usb_send_status(STATUS_OK);
                break;
            }
            case CMD_EMMC_GET_SEC_COUNT: {
                print("CMD_EMMC_GET_SEC_COUNT\r\n");
                uint32_t sec_count = emmc_get_sec_count();
                usb_send_emmc_sec_count(sec_count);
                break;
            }
            case CMD_EMMC_READ_SINGLE_BLOCK: {
                print("CMD_EMMC_READ_SINGLE_BLOCK\r\n");
                if (data_length != 4) {
                    print("CMD_EMMC_READ_SINGLE_BLOCK: data_length != 4\r\n");
                    usb_send_status(STATUS_ERROR_DATA_LENGTH);
                    break;
                }

                uint32_t lba = READ_BE32(data);

                ret = emmc_read_single_block(lba, data);
                if (ret) {
                    print("CMD_EMMC_READ_SINGLE_BLOCK: emmc_read_single_block failed\r\n");
                    usb_send_status(STATUS_ERROR);
                    break;
                }

                usb_send_emmc_read_single_block(EMMC_BLOCK_SIZE);
                break;
            }
            case CMD_EMMC_WRITE_BLOCK: {
                print("CMD_EMMC_WRITE_BLOCK\r\n");
                if (data_length != 4 + EMMC_BLOCK_SIZE) {
                    print("CMD_EMMC_WRITE_BLOCK: data_length != 4 + EMMC_BLOCK_SIZE\r\n");
                    usb_send_status(STATUS_ERROR_DATA_LENGTH);
                    break;
                }

                uint32_t lba = READ_BE32(data);

                ret = emmc_write_block(lba, data + 4);
                if (ret) {
                    print("CMD_EMMC_WRITE_BLOCK: emmc_write_block failed\r\n");
                    usb_send_status(STATUS_ERROR);
                    break;
                }

                usb_send_status(STATUS_OK);
                break;
            }
        }
    }
}