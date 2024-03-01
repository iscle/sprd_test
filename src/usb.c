#include "usb.h"
#include <string.h>

#define USB_BASE                    0x5FFF0000
#define USB_DMA_CHN_CFG(x)          (*(volatile uint32_t *) (USB_BASE + 0x1C04 + (((x) - 1) * 0x20)))
#define USB_DMA_CHN_INT(x)          (*(volatile uint32_t *) (USB_BASE + 0x1C08 + (((x) - 1) * 0x20)))
#define USB_DMA_CHN_LEN(x)          (*(volatile uint32_t *) (USB_BASE + 0x1C10 + (((x) - 1) * 0x20)))
#define USB_DMA_CHN_LLIST_PTR(x)    (*(volatile uint32_t *) (USB_BASE + 0x1C14 + (((x) - 1) * 0x20)))

#define MAX_USB_LENGTH 1024

typedef struct {
    uint32_t addr;
    uint16_t frag_len;
    uint16_t blk_len;
    uint8_t flags;
} usb_ll_t;

static __attribute__((aligned(8))) usb_ll_t usb_ll_rx;
static __attribute__((aligned(8))) usb_ll_t usb_ll_tx;
static __attribute__((aligned(64))) uint8_t usb_buf[MAX_USB_LENGTH];

extern void print(const char *s);

static uint16_t crc16(const void *data, size_t length) {
    const uint8_t *d = data;
    uint16_t crc = 0;
    for (size_t i = 0; i < length; i++) {
        crc ^= d[i] << 8;
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

void usb_rx_init() {
    if (((uint64_t) &usb_ll_rx) & 0xFFFFFFFF00000000) {
        // DMA only allows for 32 bit addresses
        // TODO: This should be a compile time check
        return;
    }

    if (((uint64_t) usb_buf) & 0xFFFFFFFF00000000) {
        // DMA only allows for 32 bit addresses
        // TODO: This should be a compile time check
        return;
    }

    usb_ll_rx.addr = (uint32_t) (uint64_t) usb_buf;
    usb_ll_rx.frag_len = 32;
    usb_ll_rx.blk_len = MAX_USB_LENGTH;
    usb_ll_rx.flags = (1 << 2) | 1; // ioc << 2 | sp << 1 | list_end << 0

    USB_DMA_CHN_INT(6 + 15) |= 1 << 4 | 1 << 2; // UNKNOWN | CHN_LLIST_INT_EN
    USB_DMA_CHN_LLIST_PTR(6 + 15) = (uint32_t) (uint64_t) &usb_ll_rx;
    USB_DMA_CHN_CFG(6 + 15) |= 1; // CHN_EN
}

static void usb_tx_init(uint16_t len) {
    if (((uint64_t) &usb_ll_tx) & 0xFFFFFFFF00000000) {
        // DMA only allows for 32 bit addresses
        // TODO: This should be a compile time check
        return;
    }

    if (((uint64_t) usb_buf) & 0xFFFFFFFF00000000) {
        // DMA only allows for 32 bit addresses
        // TODO: This should be a compile time check
        return;
    }

    usb_ll_tx.addr = (uint32_t) (uint64_t) usb_buf;
    usb_ll_tx.frag_len = 32;
    usb_ll_tx.blk_len = len;
    usb_ll_tx.flags = (1 << 2) | 1; // ioc << 2 | sp << 1 | list_end << 0

    USB_DMA_CHN_INT(5) |= 1 << 2; // CHN_LLIST_INT_EN
    USB_DMA_CHN_LLIST_PTR(5) = (uint32_t) (uint64_t) &usb_ll_tx;
    USB_DMA_CHN_CFG(5) |= 1; // CHN_EN
}

int usb_write(usb_cmd_t cmd, uint16_t data_length, const void *data) {
    if (data_length > (MAX_USB_LENGTH - 3 - 2)) {
        return -1;
    }

    usb_buf[0] = cmd;
    usb_buf[1] = (data_length >> 8) & 0xFF;
    usb_buf[2] = data_length & 0xFF;
    memcpy(usb_buf + 3, data, data_length);
    uint16_t crc = crc16(usb_buf, 3 + data_length);
    usb_buf[3 + data_length] = (crc >> 8) & 0xFF;
    usb_buf[3 + data_length + 1] = crc & 0xFF;

    usb_tx_init(3 + data_length + 2);

    // wait for the transfer to complete
    while (!(USB_DMA_CHN_INT(5) & (1 << 18))); // CHN_LLIST_INT_MASK_STS
    USB_DMA_CHN_INT(5) |= 1 << 26; // CHN_LLIST_INT_CLR
    
    return 0;
}

int usb_read(usb_cmd_t *cmd, uint16_t *data_length, void **data) {
    while (!(USB_DMA_CHN_INT(6 + 15) & (1 << 18))); // CHN_LLIST_INT_MASK_STS
    USB_DMA_CHN_INT(6 + 15) |= 1 << 28 | 1 << 26; // UNKNOWN | CHN_LLIST_INT_CLR
    
    uint16_t len = (USB_DMA_CHN_LEN(6 + 15) >> 16) & 0xFFFF;
    len = MAX_USB_LENGTH - len;
    print("Received ");
    print_dec(len);
    print(" bytes\r\n");

    if (len < 3) {
        print("len < 3\r\n");
        return -1;
    }

    *cmd = usb_buf[0];
    *data_length = (usb_buf[1] << 8) | usb_buf[2];
    if (*data_length > (MAX_USB_LENGTH - 3 - 2)) {
        print("length too long\r\n");
        return -1;
    }

    if (len != (3 + *data_length + 2)) {
        print("len != (3 + *data_length + 2)\n");
        return -1;
    }

    *data = usb_buf + 3;

    uint16_t crc = usb_buf[3 + *data_length] << 8 | usb_buf[3 + *data_length + 1];
    if (crc != crc16(usb_buf, 3 + *data_length)) {
        print("crc != crc16(usb_buf, 3 + length)\r\n");
        return -1;
    }

    usb_rx_init();
    return 0;
}

int usb_send_status(usb_status_t status) {
    uint8_t s = status;
    return usb_write(CMD_STATUS, sizeof(s), &s);
}

int usb_send_version(char *version, uint16_t length) {
    return usb_write(CMD_VERSION, length, version);
}

int usb_send_read_partition_data_start(uint64_t length) {
    uint8_t buf[8];

    buf[0] = (length >> 56) & 0xFF;
    buf[1] = (length >> 48) & 0xFF;
    buf[2] = (length >> 40) & 0xFF;
    buf[3] = (length >> 32) & 0xFF;
    buf[4] = (length >> 24) & 0xFF;
    buf[5] = (length >> 16) & 0xFF;
    buf[6] = (length >> 8) & 0xFF;
    buf[7] = length & 0xFF;

    return usb_write(CMD_EMMC_READ_PARTITION_DATA_START, sizeof(buf), buf);
}

int usb_send_read_partition_data(uint16_t length, const void *data) {
    return usb_write(CMD_EMMC_READ_PARTITION_DATA, length, data);
}

int usb_send_read_partition_data_end() {
    return usb_write(CMD_EMMC_READ_PARTITION_DATA_END, 0, NULL);
}