#include "emmc.h"
#include <stddef.h>

extern void print(const char *s);

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

#define BOOT_ACK                    (1 << 31)
#define CMD_LINE_BOOT               (1 << 30)
#define CMD_INDEX(x)                (((x) & 0x3F) << 24)
#define CMD_TYPE_NORMAL             (0 << 22)
#define CMD_TYPE_ABORT              (3 << 22)
#define DATA_PRE_SEL                (1 << 21)
#define CMD_IND_CHK_EN              (1 << 20)
#define CMD_CRC_CHK_EN              (1 << 19)
#define SUB_CMD_FLAG_MAIN           (0 << 18)
#define SUB_CMD_FLAG_SUB            (1 << 18)
#define RESP_TYPE_SEL_NO_RESPONSE   (0 << 16)
#define RESP_TYPE_SEL_136           (1 << 16)
#define RESP_TYPE_SEL_48            (2 << 16)
#define RESP_TYPE_SEL_48_CHECK      (3 << 16)
#define RESP_INT_DIS                (1 << 8)
#define RESP_ERR_CHK_EN             (1 << 7)
#define RESP_TYPE_R1                (0 << 6)
#define RESP_TYPE_R5                (1 << 6)
#define MULTI_BLK_SEL               (1 << 5)
#define DATA_DIR_SEL_WRITE          (0 << 4)
#define DATA_DIR_SEL_READ           (1 << 4)
#define AUTO_CMD_EN_DISABLE         (0 << 2)
#define AUTO_CMD_EN_CMD12           (1 << 2)
#define AUTO_CMD_EN_CMD23           (2 << 2)
#define AUTO_CMD_EN_AUTO            (3 << 2)

struct cmd_data_param {
    uint8_t *data_buf;
    uint32_t blk_len;
    uint32_t blk_num;
};

enum emmc_cmd {
    GO_IDLE_STATE,
    SEND_OP_COND,
};

// Array positions to be kept in sync with enum emmc_cmd
static uint32_t cmds[] = {
        // CMD_GO_IDLE_STATE
        CMD_INDEX(0) | CMD_TYPE_NORMAL | RESP_TYPE_SEL_NO_RESPONSE,
        // CMD_SEND_OP_COND
        CMD_INDEX(1) | CMD_TYPE_NORMAL | RESP_TYPE_SEL_48
};

void emmc_sdclk_enable(uint8_t enable) {
    if (enable) {
        EMMC_CLK_CTRL |= 1 << 2; // SDCLK_EN
    } else {
        EMMC_CLK_CTRL &= ~(1 << 2); // SDCLK_EN
    }
}

static inline void emmc_set_data_timeout(uint8_t timeout) {
    // Save original data timeout interrupt
    uint32_t tmp = EMMC_INT_ST_EN;
    // Disable data timeout interrupt
    EMMC_INT_ST_EN &= ~(1 << 20); // DATA_TIMEOUT_ERR_EN
    // Set timeout
    EMMC_CLK_CTRL &= ~(0xF << 16); // DATA_TIMEOUT_CNT
    EMMC_CLK_CTRL |= (timeout & 0xF) << 16; // DATA_TIMEOUT_CNT
    // Restore data timeout interrupt
    EMMC_INT_ST_EN = tmp;
}

static inline void emmc_set_int_en(uint32_t int_en) {
    EMMC_INT_ST_EN = int_en;
    EMMC_INT_SIG_EN = int_en;
}

static inline void emmc_reset_cmd_dat() {
    EMMC_CLK_CTRL |= (1 << 26) | (1 << 25); // SW_RST_DAT | SW_RST_CMD
    while (EMMC_CLK_CTRL & ((1 << 26) | (1 << 25))); // SW_RST_DAT | SW_RST_CMD
}

void emmc_send_cmd(enum emmc_cmd cmd, uint32_t arg, uint8_t *rsp_buf) {
    print("emmc_send_cmd: emmc_set_data_timeout\r\n");
    // Set maximum timeout value
    emmc_set_data_timeout(0xF);

    print("emmc_send_cmd: emmc_set_int_en\r\n");
    // Enable interrupts
    emmc_set_int_en((1 << 0)); // CMD_COMPLETE_EN

    EMMC_ARGUMENT = arg;
    EMMC_TR_MODE = cmds[cmd];

    print("emmc_send_cmd: wait for CMD_COMPLETE\r\n");
    while (!(EMMC_INT_ST & (1 << 0))); // CMD_COMPLETE

    print("emmc_send_cmd: emmc_reset_cmd_dat\r\n");
    emmc_reset_cmd_dat();

    if (rsp_buf == NULL)
        return;

    switch (cmds[cmd] & (0x3 << 16)) {
        case RESP_TYPE_SEL_NO_RESPONSE:
            break;
        case RESP_TYPE_SEL_136:
            ((uint32_t *) rsp_buf)[0] = EMMC_RESP0;
            ((uint32_t *) rsp_buf)[1] = EMMC_RESP2;
            ((uint32_t *) rsp_buf)[2] = EMMC_RESP4;
            ((uint32_t *) rsp_buf)[3] = EMMC_RESP6;
            break;
        case RESP_TYPE_SEL_48:
            ((uint32_t *) rsp_buf)[0] = EMMC_RESP0;
            break;
        case RESP_TYPE_SEL_48_CHECK:
            ((uint32_t *) rsp_buf)[0] = EMMC_RESP0;
            break;
    }
}

void emmc_send_cmd_data() {

}

void emmc_init() {
    print("SDHOST_Register start\r\n");
    // SDHOST_Register start
    REG_AP_APB_APB_EB |= 1 << 25; // BIT_AP_APB_EMMC_EB

    if (EMMC_CLK_CTRL & (1 << 2)) { // SDCLK_EN
        EMMC_CLK_CTRL &= ~(1 << 2); // SDCLK_EN
        // TODO: delay_ms(1);
    }

    REG_AP_APB_APB_RST |= 1 << 19; // BIT_AP_APB_EMMC_SOFT_RST
    // TODO: delay_ms(1);
    REG_AP_APB_APB_RST &= ~(1 << 19); // BIT_AP_APB_EMMC_SOFT_RST
    // SDHOST_Register end

    print("SDIO_PowerCtl start\r\n");
    // SDIO_PowerCtl start
    // SDHOST_RST start
    if (EMMC_CLK_CTRL & (1 << 2)) { // SDCLK_EN
        EMMC_CLK_CTRL &= ~(1 << 2); // SDCLK_EN
        // TODO: delay_ms(1);
    }

    EMMC_CLK_CTRL |= 1 << 24; // SW_RST_ALL
    while (EMMC_CLK_CTRL & (1 << 24)); // SW_RST_ALL
    // SDHOST_RST end

    print("SDHOST_ClkFreq_Set start\r\n");
    // SDHOST_ClkFreq_Set start
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

    print("SDHOST_InternalClk_Enable start\r\n");
    // SDHOST_InternalClk_Enable start
    EMMC_CLK_CTRL |= 1; // INT_CLK_EN
    while (!(EMMC_CLK_CTRL & 1)); // INT_CLK_STABLE
    // SDHOST_InternalClk_Enable end
    // SDHOST_SdClk_Enable start
    EMMC_CLK_CTRL |= 1 << 2; // SDCLK_EN
    // SDHOST_SdClk_Enable end

    print("SDIO_Enable64Bit start\r\n");
    // SDIO_Enable64Bit start
    EMMC_HOST_CTRL2 |= 0x20000000; // EMMC_ENABLE_64BIT
    EMMC_HOST_CTRL1 &= ~(0x18); // EMMC_SDMA_MODE
    // SDIO_Enable64Bit end

    print("CARD_SDIO_InitCard start\r\n");
    // CARD_SDIO_InitCard start
    emmc_send_cmd(GO_IDLE_STATE, 0, NULL);

    return;
}