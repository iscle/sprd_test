#include "usb.h"
#include <string.h>
#include "platform.h"
#include "uart.h"

#define USB_BASE                    0x5FFF0000
#define USB_DMA_BASE                (USB_BASE + 0x1000)
#define USB_DMA_CHN_CFG(x)          (USB_DMA_BASE + 0xc04 + ((x) - 1) * 0x20)
#define USB_DMA_CHN_INT(x)          (USB_DMA_BASE + 0xc08 + ((x) - 1) * 0x20)
#define USB_DMA_CHN_LEN(x)          (USB_DMA_BASE + 0xc10 + ((x) - 1) * 0x20)
#define USB_DMA_CHN_LLIST_PTR(x)    (USB_DMA_BASE + 0xc14 + ((x) - 1) * 0x20)

#define DMA_CHN_CFG(x)          (*(volatile uint32_t *) (USB_DMA_BASE + 0xC04 + (((x) - 1) * 0x20)))
#define DMA_CHN_INT(x)          (*(volatile uint32_t *) (USB_DMA_BASE + 0xC08 + (((x) - 1) * 0x20)))
#define DMA_CHN_LEN(x)          (*(volatile uint32_t *) (USB_DMA_BASE + 0xC10 + (((x) - 1) * 0x20)))
#define DMA_CHN_LLIST_PTR(x)    (*(volatile uint32_t *) (USB_DMA_BASE + 0xC14 + (((x) - 1) * 0x20)))

typedef struct {
    uint32_t addr;
    uint16_t frag_len;
    uint16_t blk_len;
    uint8_t flags;
} usb_ll_t;

static __attribute__((aligned(8))) usb_ll_t usb_ll_rx;
static __attribute__((aligned(8))) usb_ll_t usb_ll_tx;
__attribute__((aligned(64))) uint8_t usb_buf[USB_MAX_LENGTH];

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

    DMA_CHN_INT(6 + 15) |= 1 << 4 | 1 << 2; // CHN_USBRX_INT_EN | CHN_LLIST_INT_EN
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

static void usb_write(uint16_t cmd, uint16_t data_length) {
    WRITE_BE16(usb_buf + 0, cmd);
    WRITE_BE16(usb_buf + 2, data_length);
    usb_tx_init(4 + data_length);

    // wait for the transfer to complete
    while (!(DMA_CHN_INT(5) & (1 << 18))); // CHN_LLIST_INT_MASK_STS
    DMA_CHN_INT(5) |= 1 << 26; // CHN_LLIST_INT_CLR
}

int usb_read(uint16_t *cmd, uint16_t *data_length) {
    while (!(DMA_CHN_INT(6 + 15) & (1 << 18))); // CHN_LLIST_INT_MASK_STS
    DMA_CHN_INT(6 + 15) |= 1 << 28 | 1 << 26; // CHN_USBRX_LAST_INT_CLR | CHN_LLIST_INT_CLR
    
    uint16_t len = USB_MAX_LENGTH - ((DMA_CHN_LEN(6 + 15) >> 16) & 0xFFFF);

    usb_rx_init();

    if (len < 6) return -1;

    *cmd = READ_BE16(usb_buf + 0);
    *data_length = READ_BE16(usb_buf + 2);

    if (len != (4 + *data_length)) return -1;

    return 0;
}

void usb_send_ok() {
    usb_write(RSP_OK, 0);
}

void usb_send_error() {
    usb_write(RSP_ERROR, 0);
}

void usb_send_version(uint8_t version) {
    #define SOCRATES "Socrates"
    memcpy(USB_DATA, SOCRATES, sizeof(SOCRATES));
    WRITE_BE8(USB_DATA + sizeof(SOCRATES), version);
    usb_write(RSP_VERSION, sizeof(SOCRATES) + 1);
}

void usb_send_reg8(uint8_t val) {
    WRITE_BE8(USB_DATA, val);
    usb_write(RSP_READ_8, 1);
}

void usb_send_reg16(uint16_t val) {
    WRITE_BE16(USB_DATA, val);
    usb_write(RSP_READ_16, 2);
}

void usb_send_reg32(uint32_t val) {
    WRITE_BE32(USB_DATA, val);
    usb_write(RSP_READ_32, 4);
}

void usb_send_reg64(uint64_t val) {
    WRITE_BE64(USB_DATA, val);
    usb_write(RSP_READ_64, 8);
}

void usb_send_mmc_get_sec_count(uint32_t sec_count) {
    WRITE_BE32(USB_DATA, sec_count);
    usb_write(RSP_MMC_GET_SEC_COUNT, 4);
}

void usb_send_mmc_read_single_block(uint16_t block_size) {
    usb_write(RSP_MMC_READ_SINGLE_BLOCK, block_size);
}