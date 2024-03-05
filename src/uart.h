#ifndef UART_H
#define UART_H

#include <stdint.h>

void print_char(char c);
void print_dec(uint32_t dec);
void print_hex(uint32_t hex);
void print(const char *s);

#endif //UART_H
