#include "emmc.h"

#define EMMC_BASE                   (0x71400000)

#define EMMC_BLK_CNT                (*(volatile uint32_t *) (EMMC_BASE + 0x0))
#define EMMC_BLK_SIZE               (*(volatile uint32_t *) (EMMC_BASE + 0x4))
#define EMMC_ARGUMENT               (*(volatile uint32_t *) (EMMC_BASE + 0x8))
#define EMMC_TR_MODE                (*(volatile uint32_t *) (EMMC_BASE + 0xC))
#define EMMC_RESP0                  (*(volatile uint32_t *) (EMMC_BASE + 0x10))
#define EMMC_RESP2                  (*(volatile uint32_t *) (EMMC_BASE + 0x14))
#define EMMC_RESP4                  (*(volatile uint32_t *) (EMMC_BASE + 0x18))
#define EMMC_RESP6                  (*(volatile uint32_t *) (EMMC_BASE + 0x1C))
#define EMMC_PRES_STATE             (*(volatile uint32_t *) (EMMC_BASE + 0x24))
#define EMMC_HOST_CTRL1             (*(volatile uint32_t *) (EMMC_BASE + 0x28))
#define EMMC_CLK_CTRL               (*(volatile uint32_t *) (EMMC_BASE + 0x2C))
#define EMMC_INT_ST                 (*(volatile uint32_t *) (EMMC_BASE + 0x30))
#define EMMC_INT_ST_EN              (*(volatile uint32_t *) (EMMC_BASE + 0x34))
#define EMMC_INT_SIG_EN             (*(volatile uint32_t *) (EMMC_BASE + 0x38))
#define EMMC_HOST_CTRL2             (*(volatile uint32_t *) (EMMC_BASE + 0x3C))
#define EMMC_CAP1                   (*(volatile uint32_t *) (EMMC_BASE + 0x40))
#define EMMC_CAP2                   (*(volatile uint32_t *) (EMMC_BASE + 0x44))
#define EMMC_FORCE_EVT              (*(volatile uint32_t *) (EMMC_BASE + 0x50))
#define EMMC_ADMA_ERR               (*(volatile uint32_t *) (EMMC_BASE + 0x54))
#define EMMC_ADMA2_ADDR_L           (*(volatile uint32_t *) (EMMC_BASE + 0x58))
#define EMMC_ADMA2_ADDR_H           (*(volatile uint32_t *) (EMMC_BASE + 0x5C))
#define EMMC_ADMA3_ADDR_L           (*(volatile uint32_t *) (EMMC_BASE + 0x78))
#define EMMC_ADMA3_ADDR_H           (*(volatile uint32_t *) (EMMC_BASE + 0x7C))
#define EMMC_HOST_VER               (*(volatile uint32_t *) (EMMC_BASE + 0xFC))
#define EMMC_DLL_CFG                (*(volatile uint32_t *) (EMMC_BASE + 0x200))
#define EMMC_DLL_DLY                (*(volatile uint32_t *) (EMMC_BASE + 0x204))
#define EMMC_DLL_DLY_OFFSET         (*(volatile uint32_t *) (EMMC_BASE + 0x208))
#define EMMC_DLL_STS0               (*(volatile uint32_t *) (EMMC_BASE + 0x210))
#define EMMC_DLL_STS1               (*(volatile uint32_t *) (EMMC_BASE + 0x214))
#define EMMC_RAM_ADDR_BUF_L         (*(volatile uint32_t *) (EMMC_BASE + 0x220))
#define EMMC_RAM_ADDR_BUF_H         (*(volatile uint32_t *) (EMMC_BASE + 0x224))
#define EMMC_BLK_CNT_BUF            (*(volatile uint32_t *) (EMMC_BASE + 0x228))
#define EMMC_BLK_CNT_IO             (*(volatile uint32_t *) (EMMC_BASE + 0x22C))
#define EMMC_ADMA2_ADDR_ING_L       (*(volatile uint32_t *) (EMMC_BASE + 0x240))
#define EMMC_ADMA2_ADDR_ING_H       (*(volatile uint32_t *) (EMMC_BASE + 0x244))
#define EMMC_ADMA3_ADDR_ING_L       (*(volatile uint32_t *) (EMMC_BASE + 0x248))
#define EMMC_ADMA3_ADDR_ING_H       (*(volatile uint32_t *) (EMMC_BASE + 0x24C))
#define EMMC_BUSY_CFG               (*(volatile uint32_t *) (EMMC_BASE + 0x250))
#define EMMC_CRCERR_STS             (*(volatile uint32_t *) (EMMC_BASE + 0x254))
#define EMMC_FSM_DEBUG0             (*(volatile uint32_t *) (EMMC_BASE + 0x260))
#define EMMC_FSM_DEBUG1             (*(volatile uint32_t *) (EMMC_BASE + 0x264))
#define EMMC_FSM_DEBUG2             (*(volatile uint32_t *) (EMMC_BASE + 0x268))
#define EMMC_TRACE_CFG              (*(volatile uint32_t *) (EMMC_BASE + 0x270))
#define EMMC_TRACE_STS              (*(volatile uint32_t *) (EMMC_BASE + 0x274))
#define EMMC_TRACE_CNT0             (*(volatile uint32_t *) (EMMC_BASE + 0x278))
#define EMMC_TRACE_CNT1             (*(volatile uint32_t *) (EMMC_BASE + 0x27C))
#define EMMC_TRACE_CNT2             (*(volatile uint32_t *) (EMMC_BASE + 0x280))
#define EMMC_CARD_DEBOUNCE          (*(volatile uint32_t *) (EMMC_BASE + 0x284))
#define EMMC_CARD_PROT_CFG          (*(volatile uint32_t *) (EMMC_BASE + 0x288))
#define EMMC_DLL_BACKUP             (*(volatile uint32_t *) (EMMC_BASE + 0x28C))

#define CTL_BASE_AP_APB             (0x71000000)
#define REG_AP_APB_APB_EB           (*(volatile uint32_t *) (CTL_BASE_AP_APB + 0x0))
#define REG_AP_APB_APB_RST          (*(volatile uint32_t *) (CTL_BASE_AP_APB + 0x4))

#define CTL_BASE_AP_CLK_CORE            (0x20200000)
#define REG_AP_CLK_CORE_CGM_EMMC_2X_CFG (*(volatile uint32_t *) (CTL_BASE_AP_CLK_CORE + 0x90))

int emmc_init() {
    REG_AP_APB_APB_EB |= 1 << 25; // BIT_AP_APB_EMMC_EB

    if (EMMC_CLK_CTRL & (1 << 2)) { // SDCLK_EN
        EMMC_CLK_CTRL &= ~(1 << 2); // SDCLK_EN
    }

    REG_AP_APB_APB_RST |= 1 << 19; // BIT_AP_APB_EMMC_SOFT_RST
    // TODO: delay_ms(1);
    REG_AP_APB_APB_RST &= ~(1 << 19); // BIT_AP_APB_EMMC_SOFT_RST

    if (EMMC_CLK_CTRL & (1 << 2)) { // SDCLK_EN
        EMMC_CLK_CTRL &= ~(1 << 2); // SDCLK_EN
    }

    EMMC_CLK_CTRL |= 1 << 24; // SW_RST_ALL
    while (EMMC_CLK_CTRL & (1 << 24)); // SW_RST_ALL

    REG_AP_CLK_CORE_CGM_EMMC_2X_CFG = 0x03; // AP_CLK_FREQ_384M

    uint32_t sdioClk = 400000; // 400KHz
    uint32_t clkDiv = 384000000 / (sdioClk * 2);
    if ((384000000 / clkDiv) > (sdioClk * 2))
        clkDiv++;
    if (clkDiv % 2)
        clkDiv = (clkDiv + 1) / 2;
    else
        clkDiv = clkDiv / 2;
    uint32_t tmpReg = EMMC_CLK_CTRL;
    tmpReg &= (~(0x3ff << 6));
    tmpReg |= ((clkDiv >> 8) & 0x3) << 6;
    tmpReg |= (clkDiv & 0xff) << 8;
    EMMC_CLK_CTRL = tmpReg;

    EMMC_CLK_CTRL |= 1; // INT_CLK_EN
    while (!(EMMC_CLK_CTRL & 1)); // INT_CLK_STABLE
    EMMC_CLK_CTRL |= 1 << 2; // SDCLK_EN

    EMMC_HOST_CTRL2 |= 0x20000000; // EMMC_ENABLE_64BIT
    EMMC_HOST_CTRL1 &= ~(0x18); // EMMC_SDMA_MODE

    return 0;
}