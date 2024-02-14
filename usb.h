#ifndef USB_H
#define USB_H

void usb_rx_init();
void usb_write(const unsigned int *buf, unsigned short len);
unsigned short usb_read(const unsigned char **buf);

#endif //USB_H
