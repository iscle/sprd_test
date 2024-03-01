#ifndef USB_H
#define USB_H

#include <stdint.h>

typedef enum {
    CMD_STATUS = 0x00,
    CMD_VERSION = 0x01,

    // EMMC
    CMD_EMMC_INIT = 0x02,
    CMD_EMMC_READ = 0x03,
    CMD_EMMC_WRITE = 0x04,
    CMD_EMMC_READ_PARTITION = 0x05,
    CMD_EMMC_READ_PARTITION_DATA_START = 0x06,
    CMD_EMMC_READ_PARTITION_DATA = 0x07,
    CMD_EMMC_READ_PARTITION_DATA_END = 0x08,

    CMD_EXIT = 0xFF,
} usb_cmd_t;

typedef enum {
    STATUS_OK = 0x00,
    STATUS_ERROR = 0x01,
} usb_status_t;

void usb_rx_init();
int usb_write(usb_cmd_t cmd, uint16_t data_length, const void *data);
int usb_read(usb_cmd_t *cmd, uint16_t *data_length, void **data);
int usb_send_status(usb_status_t status);
int usb_send_version(char *version, uint16_t length);
int usb_send_read_partition_data_start(uint64_t length);
int usb_send_read_partition_data(uint16_t length, const void *data);
int usb_send_read_partition_data_end();

#endif //USB_H
