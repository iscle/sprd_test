#include "usb.h"
#include <string.h>
#include "endianness.h"

#define USB_BASE                0x5FFF0000
#define DMA_BASE                (USB_BASE + 0x1000)
#define DMA_CHN_CFG(x)          (*(volatile uint32_t *) (DMA_BASE + 0xC04 + (((x) - 1) * 0x20)))
#define DMA_CHN_INT(x)          (*(volatile uint32_t *) (DMA_BASE + 0xC08 + (((x) - 1) * 0x20)))
#define DMA_CHN_LEN(x)          (*(volatile uint32_t *) (DMA_BASE + 0xC10 + (((x) - 1) * 0x20)))
#define DMA_CHN_LLIST_PTR(x)    (*(volatile uint32_t *) (DMA_BASE + 0xC14 + (((x) - 1) * 0x20)))

typedef struct {
    uint32_t addr;
    uint16_t frag_len;
    uint16_t blk_len;
    uint8_t flags;
} usb_ll_t;

static __attribute__((aligned(8))) usb_ll_t usb_ll_rx;
static __attribute__((aligned(8))) usb_ll_t usb_ll_tx;

#define USB_DATA_OFFSET 4
static __attribute__((aligned(64))) uint8_t usb_buf[USB_MAX_LENGTH];

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
    usb_ll_rx.blk_len = USB_MAX_LENGTH;
    usb_ll_rx.flags = (1 << 2) | 1; // ioc << 2 | sp << 1 | list_end << 0

    DMA_CHN_INT(6 + 15) |= 1 << 4 | 1 << 2; // UNKNOWN | CHN_LLIST_INT_EN
    DMA_CHN_LLIST_PTR(6 + 15) = (uint32_t) (uint64_t) &usb_ll_rx;
    DMA_CHN_CFG(6 + 15) |= 1; // CHN_EN
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

    DMA_CHN_INT(5) |= 1 << 2; // CHN_LLIST_INT_EN
    DMA_CHN_LLIST_PTR(5) = (uint32_t) (uint64_t) &usb_ll_tx;
    DMA_CHN_CFG(5) |= 1; // CHN_EN
}

inline void *usb_get_data() {
    return usb_buf + USB_DATA_OFFSET;
}

int usb_write(uint16_t cmd, uint16_t data_length) {
    if (data_length > USB_MAX_DATA_LENGTH) {
        return -1;
    }

    WRITE_BE16(usb_buf + 0, cmd);
    WRITE_BE16(usb_buf + 2, data_length);
    // data should be set by the caller using usb_get_data
    uint16_t crc = crc16(usb_buf, 4 + data_length);
    WRITE_BE16(usb_buf + 4 + data_length, crc);

    usb_tx_init(4 + data_length + 2);

    // wait for the transfer to complete
    while (!(DMA_CHN_INT(5) & (1 << 18))); // CHN_LLIST_INT_MASK_STS
    DMA_CHN_INT(5) |= 1 << 26; // CHN_LLIST_INT_CLR
    
    return 0;
}

int usb_read(uint16_t *cmd, uint16_t *data_length) {
    while (!(DMA_CHN_INT(6 + 15) & (1 << 18))); // CHN_LLIST_INT_MASK_STS
    DMA_CHN_INT(6 + 15) |= 1 << 28 | 1 << 26; // UNKNOWN | CHN_LLIST_INT_CLR
    
    uint16_t len = USB_MAX_LENGTH - ((DMA_CHN_LEN(6 + 15) >> 16) & 0xFFFF);
    print("Received ");
    print_dec(len);
    print(" bytes\r\n");

    usb_rx_init();

    if (len < 6) {
        print("len < 6\r\n");
        return -1;
    }

    *cmd = READ_BE16(usb_buf + 0);
    *data_length = READ_BE16(usb_buf + 2);
    if (*data_length > USB_MAX_DATA_LENGTH) {
        print("data length too long\r\n");
        return -1;
    }

    if (len != (4 + *data_length + 2)) {
        print("len != (4 + *data_length + 2)\n");
        return -1;
    }

    uint16_t crc = READ_BE16(usb_buf + 4 + *data_length);
    if (crc != crc16(usb_buf, 4 + *data_length)) {
        print("crc != crc16(usb_buf, 4 + length)\r\n");
        return -1;
    }

    return 0;
}

void usb_send_status(uint16_t status) {
    WRITE_BE16(usb_get_data(), status);
    usb_write(CMD_STATUS, 2);
}

void usb_send_version(char *version, uint16_t length) {
    memcpy(usb_get_data(), version, length);
    usb_write(CMD_VERSION, length);
}

void usb_send_emmc_read_single_block(uint16_t block_size) {
    usb_write(CMD_EMMC_READ_SINGLE_BLOCK, block_size);
}