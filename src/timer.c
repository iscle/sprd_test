#include "timer.h"

#define SYST_BASE       (0x322E0000)
#define SYST_ALM        (*(volatile unsigned int *) (SYST_BASE + 0x0))
#define SYST_CNT        (*(volatile unsigned int *) (SYST_BASE + 0x4))
#define SYST_INT        (*(volatile unsigned int *) (SYST_BASE + 0x8))
#define SYST_CNT_SHDW   (*(volatile unsigned int *) (SYST_BASE + 0xC))

void delay_ms(uint32_t ms) {
    uint32_t start = SYST_CNT_SHDW;
    uint32_t now = start;
    while (now - start < ms) {
        now = SYST_CNT_SHDW;
    }
}