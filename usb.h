#ifndef USB_H
#define USB_H

#include <stdint.h>

void usb_rx_init();
void usb_write(const uint32_t *buf, uint16_t len);
uint16_t usb_read(const uint8_t **buf);

#endif //USB_H
