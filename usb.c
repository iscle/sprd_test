#include "usb.h"

#define USB_BASE                    (0x5FFF0000)
#define USB_DMA_CHN_CFG(x)          (*(volatile uint32_t *) (USB_BASE + 0x1C04 + (((x) - 1) * 0x20)))
#define USB_DMA_CHN_INT(x)          (*(volatile uint32_t *) (USB_BASE + 0x1C08 + (((x) - 1) * 0x20)))
#define USB_DMA_CHN_LEN(x)          (*(volatile uint32_t *) (USB_BASE + 0x1C10 + (((x) - 1) * 0x20)))
#define USB_DMA_CHN_LLIST_PTR(x)    (*(volatile uint32_t *) (USB_BASE + 0x1C14 + (((x) - 1) * 0x20)))

#define MAX_RECV_LEN (64 * 64)

struct usb_ll_entry {
    uint32_t addr;
    uint16_t frag_len;
    uint16_t blk_len;
    uint8_t flags;
};

static __attribute__((aligned(8))) struct usb_ll_entry linklist_ram_tx;
static __attribute__((aligned(8))) struct usb_ll_entry linklist_ram_rx;
static __attribute__((aligned(64))) uint8_t usb_in_ep_buf[MAX_RECV_LEN];

extern void print(const char *s);

static void usb_memset(void *s, int c, int n) {
    int i;
    for (i = 0; i < n; i++) {
        ((uint8_t *) s)[i] = c;
    }
}

void usb_rx_init() {
    if (((uint64_t) &linklist_ram_rx) & 0xFFFFFFFF00000000) {
        // DMA only allows for 32 bit addresses
        print("usb_read: linklist_ram_rx out of range, DMA only allows for 32 bit addresses\r\n");
        return;
    }

    usb_memset(&linklist_ram_rx, 0, sizeof(linklist_ram_rx));

    linklist_ram_rx.addr = (uint32_t) usb_in_ep_buf;
    linklist_ram_rx.frag_len = 32;
    linklist_ram_rx.blk_len = MAX_RECV_LEN;
    linklist_ram_rx.flags = (1 << 2) | 1; // ioc << 2 | sp << 1 | list_end << 0

    USB_DMA_CHN_INT(6 + 15) |= 1 << 4 | 1 << 2; // UNKNOWN | CHN_LLIST_INT_EN
    USB_DMA_CHN_LLIST_PTR(6 + 15) = (uint32_t) &linklist_ram_rx;
    USB_DMA_CHN_CFG(6 + 15) |= 1; // CHN_EN
}

/*
 * Write data to the USB FIFO (endpoint 5)
 *
 * This call is blocking and will not return until the data has been written to the FIFO.
 */
void usb_write(const uint32_t *buf, uint16_t len) {
    if (((uint64_t) buf) & 0xFFFFFFFF00000000) {
        // DMA only allows for 32 bit addresses
        print("usb_write: buf out of range, DMA only allows for 32 bit addresses\r\n");
        return;
    }

    if (((uint64_t) &linklist_ram_tx) & 0xFFFFFFFF00000000) {
        // DMA only allows for 32 bit addresses
        print("usb_write: linklist_ram_tx out of range, DMA only allows for 32 bit addresses\r\n");
        return;
    }

    usb_memset(&linklist_ram_tx, 0, sizeof(linklist_ram_tx));

    linklist_ram_tx.addr = (uint32_t) buf;
    linklist_ram_tx.frag_len = 32;
    linklist_ram_tx.blk_len = len;
    linklist_ram_tx.flags = (1 << 2) | 1; // ioc << 2 | sp << 1 | list_end << 0

    USB_DMA_CHN_INT(5) |= 1 << 2; // CHN_LLIST_INT_EN
    USB_DMA_CHN_LLIST_PTR(5) = (uint32_t) &linklist_ram_tx;
    USB_DMA_CHN_CFG(5) |= 1; // CHN_EN

    while (!(USB_DMA_CHN_INT(5) & (1 << 18))); // CHN_LLIST_INT_MASK_STS
    USB_DMA_CHN_INT(5) |= 1 << 26; // CHN_LLIST_INT_CLR
}

uint16_t usb_read(const uint8_t **buf) {
    if (((uint64_t) usb_in_ep_buf) & 0xFFFFFFFF00000000) {
        // DMA only allows for 32 bit addresses
        print("usb_read: usb_in_ep_buf out of range, DMA only allows for 32 bit addresses\r\n");
        return 0;
    }

    while (!(USB_DMA_CHN_INT(6 + 15) & (1 << 18))); // CHN_LLIST_INT_MASK_STS
    USB_DMA_CHN_INT(6 + 15) |= 1 << 28 | 1 << 26; // UNKNOWN | CHN_LLIST_INT_CLR

    print("Computer says: ");
    print(usb_in_ep_buf);
    uint16_t len = (USB_DMA_CHN_LEN(6 + 15) >> 16) & 0xFFFF;

    usb_rx_init();

    *buf = usb_in_ep_buf;
    return len;
}