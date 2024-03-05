#ifndef USB_H
#define USB_H

#include <stdint.h>

#define USB_MAX_LENGTH 4096
#define USB_MAX_DATA_LENGTH (USB_MAX_LENGTH - 4 - 2)

typedef enum {
    CMD_STATUS = 0x0000,
    CMD_VERSION = 0x0001,

    // EMMC
    CMD_EMMC_INIT = 0x0002,
    CMD_EMMC_SWITCH = 0x0003,
    CMD_EMMC_GET_SEC_COUNT = 0x0004,
    CMD_EMMC_READ_SINGLE_BLOCK = 0x0005,
    CMD_EMMC_WRITE_BLOCK = 0x0006,

    CMD_EXIT = 0xFFFF,
} usb_cmd_t;

typedef enum {
    STATUS_OK = 0x0000,
    STATUS_ERROR = 0x0001,
    STATUS_ERROR_DATA_LENGTH = 0x0002,
} usb_status_t;

void usb_rx_init();
void *usb_get_data();
int usb_write(uint16_t cmd, uint16_t data_length);
int usb_read(uint16_t *cmd, uint16_t *data_length);
void usb_send_status(uint16_t status);
void usb_send_version(char *version, uint16_t length);
void usb_send_emmc_sec_count(uint32_t sec_count);
void usb_send_emmc_read_single_block(uint16_t block_size);

#endif //USB_H
