#ifndef USB_H
#define USB_H

#include <stdint.h>

typedef enum {
    REQ_VERSION = 0x0000      /* 0x0000 */,

    /* Registers */
    REQ_READ_8                /* 0x0001 */,
    REQ_WRITE_8               /* 0x0002 */,
    REQ_READ_16               /* 0x0003 */,
    REQ_WRITE_16              /* 0x0004 */,
    REQ_READ_32               /* 0x0005 */,
    REQ_WRITE_32              /* 0x0006 */,
    REQ_READ_64               /* 0x0007 */,
    REQ_WRITE_64              /* 0x0008 */,

    /* MMC */
    REQ_MMC_INIT              /* 0x0009 */,
    REQ_MMC_SWITCH            /* 0x000a */,
    REQ_MMC_GET_SEC_COUNT     /* 0x000b */,
    REQ_MMC_READ_SINGLE_BLOCK /* 0x000c */,
    REQ_MMC_WRITE_BLOCK       /* 0x000d */,

    RSP_OK = 0x8000           /* 0x8000 */,
    RSP_ERROR                 /* 0x8001 */,

    RSP_VERSION               /* 0x8002 */,

    /* Registers */
    RSP_READ_8                /* 0x8003 */,
    RSP_READ_16               /* 0x8004 */,
    RSP_READ_32               /* 0x8005 */,
    RSP_READ_64               /* 0x8006 */,

    /* MMC */
    RSP_MMC_GET_SEC_COUNT     /* 0x8007 */,
    RSP_MMC_READ_SINGLE_BLOCK /* 0x8008 */,
} usb_cmd_t;

#define USB_MAX_LENGTH 1024
#define USB_MAX_DATA_LENGTH (USB_MAX_LENGTH - 4)
#define USB_DATA_OFFSET 4

extern uint8_t usb_buf[USB_MAX_LENGTH];
#define USB_DATA (usb_buf + USB_DATA_OFFSET)

void usb_rx_init();
int usb_read(uint16_t *cmd, uint16_t *data_length);
void usb_send_ok();
void usb_send_error();
void usb_send_version(uint8_t version);
void usb_send_reg8(uint8_t val);
void usb_send_reg16(uint16_t val);
void usb_send_reg32(uint32_t val);
void usb_send_reg64(uint64_t val);
void usb_send_emmc_sec_count(uint32_t sec_count);
void usb_send_emmc_read_single_block(uint16_t block_size);

#endif //USB_H
