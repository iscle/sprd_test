#include "emmc.h"
#include <stddef.h>
#include "efi.h"
#include "timer.h"

#define EMMC_DEBUG
#ifdef EMMC_DEBUG
void print_char(char c);
extern void print_dec(uint32_t dec);
extern void print_hex(uint32_t hex);
extern void print(const char *s);
#else
#define print_char(x)
#define print_dec(x)
#define print_hex(x)
#define print(x)
#endif

#define BASE_CLK    (384000000)

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

#define HW_RST_CARD                 (1 << 27)
#define SW_RST_DAT                  (1 << 26)
#define SW_RST_CMD                  (1 << 25)
#define SW_RST_ALL                  (1 << 24)
#define DATA_TIMEOUT_CNT(x)         (((x) & 0xF) << 16)
//#define Freq_div(x)                 (((x) & 0xFF) << 8)
#define SDCLK_EN                    (1 << 2)
#define INT_CLK_STABLE              (1 << 1)
#define INT_CLK_EN                  (1 << 0)

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
#define MULT_BLK_SEL               (1 << 5)
#define DATA_DIR_SEL                (1 << 4)
#define AUTO_CMD_EN_DISABLE         (0 << 2)
#define AUTO_CMD_EN_CMD12           (1 << 2)
#define AUTO_CMD_EN_CMD23           (2 << 2)
#define AUTO_CMD_EN_AUTO            (3 << 2)
#define BLK_CNT_EN                  (1 << 1)
#define DMA_EN                      (1 << 0)

#define AXI_RESP_ERR        (1 << 28)
#define RESP_ERROR          (1 << 27)
#define ADMA_ERROR          (1 << 25)
#define AUTO_CMD12_ERR      (1 << 24)
#define DATA_END_BIT_ERR    (1 << 22)
#define DATA_CRC_ERR        (1 << 21)
#define DATA_TIMEOUT_ERR    (1 << 20)
#define CMD_IND_ERR         (1 << 19)
#define CMD_END_BIT_ERR     (1 << 18)
#define CMD_CRC_ERROR       (1 << 17)
#define CMD_TIMEOUT_ERR     (1 << 16)
#define ERR_INT             (1 << 15)
#define ADMA3_COMPLETE      (1 << 14)
#define CARD_INT            (1 << 8)
#define DMA_INT             (1 << 3)
#define TR_COMPLETE         (1 << 1)
#define CMD_COMPLETE        (1 << 0)

#define AXI_RESP_ERR_EN     (1 << 28)
#define RESP_ERROR_EN       (1 << 27)
#define ADMA_ERROR_EN       (1 << 25)
#define AUTO_CMD12_ERR_EN   (1 << 24)
#define DATA_END_BIT_ERR_EN (1 << 22)
#define DATA_CRC_ERR_EN     (1 << 21)
#define DATA_TIMEOUT_ERR_EN (1 << 20)
#define CMD_IND_ERR_EN      (1 << 19)
#define CMD_END_BIT_ERR_EN  (1 << 18)
#define CMD_CRC_ERROR_EN    (1 << 17)
#define CMD_TIMEOUT_ERR_EN  (1 << 16)
#define ADMA3_COMPLETE_EN   (1 << 14)
#define CARD_INT_EN         (1 << 8)
#define DMA_INT_EN          (1 << 3)
#define TR_COMPLETE_EN      (1 << 1)
#define CMD_COMPLETE_EN     (1 << 0)

// EMMC_HOST_CTRL2
#define _64_BIT_ADDR_EN     (1 << 29)
#define HOST_VER_4_EN       (1 << 28)
#define CMD23_ENABLE        (1 << 27)
#define ADMA2_LEN_MODE      (1 << 26)
//#define UHS_MODE(x)                 (((x) & 0x7) << 16)
#define CMD_NOT_ISS_ERR     (1 << 7)
#define ACMD_IDX_ERR        (1 << 4)
#define ACMD_END_BIT_ERR    (1 << 3)
#define ACMD_CRC_ERR        (1 << 2)
#define ACMD_TIMEOUT_ERR    (1 << 1)
#define ACMD12_NOT_EXEC     (1 << 0)

typedef enum {
    EMMC_RESP_TYPE_NO_RESPONSE,
    EMMC_RESP_TYPE_R1,
    EMMC_RESP_TYPE_R1B,
    EMMC_RESP_TYPE_R2,
    EMMC_RESP_TYPE_R3,
    EMMC_RESP_TYPE_R4,
    EMMC_RESP_TYPE_R5,
} emmc_resp_type;

typedef enum {
    EMMC_DATA_NONE,
    EMMC_DATA_READ,
    EMMC_DATA_WRITE,
    EMMC_DATA_READ_WRITE,
} emmc_data_type;

// bc = broadcast commands, no response
// bcr = broadcast commands with response
// ac = addressed commands, no data transfer on DAT lines
// adtc = addressed data transfer commands, data transfer on DAT lines
typedef struct {
    emmc_cmd_t cmd;
    uint32_t index;
    emmc_resp_type resp_type;
    emmc_data_type data_type;
} emmc_cmd_info;

// Array positions to be kept in sync with enum emmc_cmd
static emmc_cmd_info cmds[] = {
        {EMMC_CMD_GO_IDLE_STATE, 0, EMMC_RESP_TYPE_NO_RESPONSE, EMMC_DATA_NONE},
        {EMMC_CMD_SEND_OP_COND, 1, EMMC_RESP_TYPE_R3, EMMC_DATA_NONE},
        {EMMC_CMD_ALL_SEND_CID, 2, EMMC_RESP_TYPE_R2, EMMC_DATA_NONE},
        {EMMC_CMD_SET_RELATIVE_ADDR, 3, EMMC_RESP_TYPE_R1, EMMC_DATA_NONE},
        {EMMC_CMD_SET_DSR, 4, EMMC_RESP_TYPE_NO_RESPONSE, EMMC_DATA_NONE},
        {EMMC_CMD_SLEEP_AWAKE, 5, EMMC_RESP_TYPE_R1B, EMMC_DATA_NONE},
        {EMMC_CMD_SWITCH, 6, EMMC_RESP_TYPE_R1B, EMMC_DATA_NONE},
        {EMMC_CMD_SELECT_DESELECT_CARD, 7, EMMC_RESP_TYPE_R1, EMMC_DATA_NONE},
        {EMMC_CMD_SEND_EXT_CSD, 8, EMMC_RESP_TYPE_R1, EMMC_DATA_READ},
        {EMMC_CMD_SEND_CSD, 9, EMMC_RESP_TYPE_R2, EMMC_DATA_NONE},
        {EMMC_CMD_SEND_CID, 10, EMMC_RESP_TYPE_R2, EMMC_DATA_NONE},
        {EMMC_CMD_READ_DAT_UNTIL_STOP, 11, EMMC_RESP_TYPE_R1, EMMC_DATA_READ},
        {EMMC_CMD_STOP_TRANSMISSION, 12, EMMC_RESP_TYPE_R1B, EMMC_DATA_NONE},
        {EMMC_CMD_SEND_STATUS, 13, EMMC_RESP_TYPE_R1, EMMC_DATA_NONE},
        {EMMC_CMD_BUSTEST_R, 14, EMMC_RESP_TYPE_R1, EMMC_DATA_READ},
        {EMMC_CMD_GO_INACTIVE_STATE, 15, EMMC_RESP_TYPE_NO_RESPONSE, EMMC_DATA_NONE},
        {EMMC_CMD_SET_BLOCKLEN, 16, EMMC_RESP_TYPE_R1, EMMC_DATA_NONE},
        {EMMC_CMD_READ_SINGLE_BLOCK, 17, EMMC_RESP_TYPE_R1, EMMC_DATA_READ},
        {EMMC_CMD_READ_MULTIPLE_BLOCK, 18, EMMC_RESP_TYPE_R1, EMMC_DATA_READ},
        {EMMC_CMD_BUSTEST_W, 19, EMMC_RESP_TYPE_R1, EMMC_DATA_WRITE},
        {EMMC_CMD_WRITE_DAT_UNTIL_STOP, 20, EMMC_RESP_TYPE_R1, EMMC_DATA_WRITE},
        {EMMC_CMD_SET_BLOCK_COUNT, 23, EMMC_RESP_TYPE_R1, EMMC_DATA_NONE},
        {EMMC_CMD_WRITE_BLOCK, 24, EMMC_RESP_TYPE_R1, EMMC_DATA_WRITE},
        {EMMC_CMD_WRITE_MULTIPLE_BLOCK, 25, EMMC_RESP_TYPE_R1, EMMC_DATA_WRITE},
        {EMMC_CMD_PROGRAM_CID, 26, EMMC_RESP_TYPE_R1, EMMC_DATA_WRITE},
        {EMMC_CMD_PROGRAM_CSD, 27, EMMC_RESP_TYPE_R1, EMMC_DATA_WRITE},
        {EMMC_CMD_SET_WRITE_PROT, 28, EMMC_RESP_TYPE_R1B, EMMC_DATA_NONE},
        {EMMC_CMD_CLR_WRITE_PROT, 29, EMMC_RESP_TYPE_R1B, EMMC_DATA_NONE},
        {EMMC_CMD_SEND_WRITE_PROT, 30, EMMC_RESP_TYPE_R1, EMMC_DATA_READ},
        {EMMC_CMD_ERASE_GROUP_START, 35, EMMC_RESP_TYPE_R1, EMMC_DATA_NONE},
        {EMMC_CMD_ERASE_GROUP_END, 36, EMMC_RESP_TYPE_R1, EMMC_DATA_NONE},
        {EMMC_CMD_ERASE, 38, EMMC_RESP_TYPE_R1B, EMMC_DATA_NONE},
        {EMMC_CMD_FAST_IO, 39, EMMC_RESP_TYPE_R4, EMMC_DATA_NONE},
        {EMMC_CMD_GO_IRQ_STATE, 40, EMMC_RESP_TYPE_R5, EMMC_DATA_NONE},
        {EMMC_CMD_LOCK_UNLOCK, 42, EMMC_RESP_TYPE_R1, EMMC_DATA_WRITE},
        {EMMC_CMD_APP_CMD, 55, EMMC_RESP_TYPE_R1, EMMC_DATA_NONE},
};

static uint32_t sec_count = 0;

static void *memcpy(void *dest, const void *src, long unsigned int n) {
    char *csrc = (char *) src;
    char *cdest = (char *) dest;

    for (int i = 0; i < n; i++) {
        cdest[i] = csrc[i];
    }

    return dest;
}

static void emmc_sdclk_enable(uint8_t enable) {
    if (enable) {
        if (!(EMMC_CLK_CTRL & SDCLK_EN)) {
            EMMC_CLK_CTRL |= SDCLK_EN;
            delay_ms(1);
        }
    } else {
        EMMC_CLK_CTRL &= ~SDCLK_EN;
    }
}

static inline void emmc_set_data_timeout(uint8_t timeout) {
    EMMC_CLK_CTRL &= ~(0xF << 16); // DATA_TIMEOUT_CNT_MASK
    EMMC_CLK_CTRL |= DATA_TIMEOUT_CNT(timeout);
}

static inline void emmc_set_int_en(uint32_t int_en) {
    EMMC_INT_ST_EN = int_en;
    EMMC_INT_SIG_EN = int_en;
}

static inline void emmc_reset_cmd_dat() {
    EMMC_CLK_CTRL |= SW_RST_DAT | SW_RST_CMD;
    while (EMMC_CLK_CTRL & (SW_RST_DAT | SW_RST_CMD));
}

int emmc_send_cmd(emmc_cmd_t cmd, uint32_t argument, uint32_t *rsp_buf) {
    emmc_cmd_info *cmd_info = &cmds[cmd];

    // Set maximum timeout value
    emmc_set_data_timeout(0xF);

    // Clear interrupts
    EMMC_INT_ST = 0xFFFFFFFF;
    // Enable interrupts
    EMMC_INT_ST_EN = 0xFFFFFFFF;
    EMMC_INT_SIG_EN = 0xFFFFFFFF;

    EMMC_ARGUMENT = argument;
    
    uint32_t tr_mode = 0;
    tr_mode |= CMD_INDEX(cmd_info->index);
    tr_mode |= CMD_TYPE_NORMAL;

    if (cmd_info->data_type != EMMC_DATA_NONE) {
        tr_mode |= DATA_PRE_SEL;
    }

//    tr_mode |= CMD_IND_CHK_EN;
//    tr_mode |= CMD_CRC_CHK_EN;

    switch (cmd_info->resp_type) {
        case EMMC_RESP_TYPE_NO_RESPONSE:
            tr_mode |= RESP_TYPE_SEL_NO_RESPONSE;
            break;
        case EMMC_RESP_TYPE_R1:
            tr_mode |= RESP_TYPE_SEL_48;
            break;
        case EMMC_RESP_TYPE_R1B:
            tr_mode |= RESP_TYPE_SEL_48_CHECK;
            break;
        case EMMC_RESP_TYPE_R2:
            tr_mode |= RESP_TYPE_SEL_136;
            break;
        case EMMC_RESP_TYPE_R3:
            tr_mode |= RESP_TYPE_SEL_48;
            break;
        case EMMC_RESP_TYPE_R4:
            tr_mode |= RESP_TYPE_SEL_48;
            break;
        case EMMC_RESP_TYPE_R5:
            tr_mode |= RESP_TYPE_SEL_48;
            break;
    }

    if (cmd_info->index == 18 || cmd_info->index == 25) {
        tr_mode |= MULT_BLK_SEL;
    }

    if (cmd_info->data_type == EMMC_DATA_READ) {
        tr_mode |= DATA_DIR_SEL;
    }

    tr_mode |= BLK_CNT_EN;
    tr_mode |= DMA_EN;
    
    EMMC_TR_MODE = tr_mode;

    while (!(EMMC_INT_ST & CMD_COMPLETE));
    if (cmd_info->data_type != EMMC_DATA_NONE) {
        while (!(EMMC_INT_ST & TR_COMPLETE));
    }
    print("EMMC_INT_ST: ");
    print_hex(EMMC_INT_ST);
    print("\r\n");

    emmc_reset_cmd_dat();

    if (rsp_buf == NULL)
        return 0;
    
    switch (cmd_info->resp_type) {
        case EMMC_RESP_TYPE_NO_RESPONSE:
            break;
        case EMMC_RESP_TYPE_R1:
            ((uint32_t *) rsp_buf)[0] = EMMC_RESP0;
            break;
        case EMMC_RESP_TYPE_R1B:
            ((uint32_t *) rsp_buf)[0] = EMMC_RESP0;
            break;
        case EMMC_RESP_TYPE_R2:
            ((uint32_t *) rsp_buf)[0] = EMMC_RESP0;
            ((uint32_t *) rsp_buf)[1] = EMMC_RESP2;
            ((uint32_t *) rsp_buf)[2] = EMMC_RESP4;
            ((uint32_t *) rsp_buf)[3] = EMMC_RESP6;
            break;
        case EMMC_RESP_TYPE_R3:
            ((uint32_t *) rsp_buf)[0] = EMMC_RESP0;
            break;
        case EMMC_RESP_TYPE_R4:
            ((uint32_t *) rsp_buf)[0] = EMMC_RESP0;
            break;
        case EMMC_RESP_TYPE_R5:
            ((uint32_t *) rsp_buf)[0] = EMMC_RESP0;
            break;
    }

    return 0;
}

int emmc_send_cmd_data(emmc_cmd_t cmd, uint32_t argument, uint8_t *blocks, uint32_t blk_size, uint32_t blk_count, uint32_t *rsp_buf) {
    EMMC_BLK_CNT = blk_count;
    EMMC_BLK_SIZE = blk_size & 0xFFF;
    EMMC_ADMA2_ADDR_L = (uint32_t) ((uint64_t) blocks) & 0xFFFFFFFF;
    EMMC_ADMA2_ADDR_H = (uint32_t) (((uint64_t) blocks) >> 32) & 0xFFFFFFFF;
    return emmc_send_cmd(cmd, argument, rsp_buf);
}

void emmc_read() {
    uint8_t buf[512];
    // read efi gpt lba 1
    emmc_send_cmd_data(EMMC_CMD_READ_SINGLE_BLOCK, 1, buf, 512, 1, NULL);

    gpt_header_t gpt_header;
    memcpy(&gpt_header, buf, sizeof(gpt_header));

    if (gpt_header.signature != 0x5452415020494645) {
        print("Invalid GPT signature\r\n");
        return;
    }

    // partition count
    print("gpt_header.number_of_partition_entries = ");
    print_dec(gpt_header.number_of_partition_entries);
    print("\r\n");

    // partition entry size
    print("gpt_header.size_of_partition_entry = ");
    print_dec(gpt_header.size_of_partition_entry);
    print("\r\n");

    uint32_t entries_per_block = 512 / gpt_header.size_of_partition_entry;
    for (uint32_t i = 0; i < gpt_header.number_of_partition_entries; i += entries_per_block) {
        uint32_t lba = gpt_header.partition_entry_lba;
        lba += i * gpt_header.size_of_partition_entry / 512;
        emmc_send_cmd_data(EMMC_CMD_READ_SINGLE_BLOCK, lba, buf, 512, 1, NULL);

        for (uint32_t j = 0; j < entries_per_block; j++) {
            gpt_entry_t gpt_entry;
            memcpy(&gpt_entry, buf + j * gpt_header.size_of_partition_entry, sizeof(gpt_entry));

            if (gpt_entry.starting_lba == 0) {
                continue;
            }

            print("Partition ");
            print_dec(i + j);
            print(":\r\n");
            print("  Partition type GUID: ");
            for (int k = 0; k < 16; k++) {
                print_hex(gpt_entry.partition_type_guid[k]);
            }
            print("\r\n");
            print("  Unique partition GUID: ");
            for (int k = 0; k < 16; k++) {
                print_hex(gpt_entry.unique_partition_guid[k]);
            }
            print("\r\n");
            print("  Starting LBA: ");
            print_dec(gpt_entry.starting_lba);
            print("\r\n");
            print("  Ending LBA: ");
            print_dec(gpt_entry.ending_lba);
            print("\r\n");
            print("  Attributes: ");
            print_hex(gpt_entry.attributes);
            print("\r\n");
            print("  Partition name: ");
            for (int k = 0; k < 36; k++) {
                print_char(gpt_entry.partition_name[k]);
            }
            print("\r\n");
        }
    }
}

static int emmc_switch(uint8_t index, uint8_t value, uint32_t *rsp_buf) {
    int ret;
    uint32_t argument = 0;

    argument |= 3 << 24; // Access = Command set
    argument |= index << 16;
    argument |= value << 8;

    ret = emmc_send_cmd(EMMC_CMD_SWITCH, argument, rsp_buf);
    if (ret) {
        return ret;
    }
    while (!(EMMC_PRES_STATE & (1 << 20))); // Wait for not busy

    return ret;
}

static void emmc_set_frequency(uint32_t frequency) {
    uint32_t freq_div = 0;

    // base_clk = 384000000
    // base_clk / 2n = freq
    // 2n = base_clk / freq
    // n = base_clk / (freq * 2)

    if (frequency < BASE_CLK) {
        freq_div = BASE_CLK / (frequency * 2);
        if ((BASE_CLK / freq_div) > (frequency * 2))
            freq_div++;
    }

    EMMC_CLK_CTRL &= ~(0x3FF << 6);
    EMMC_CLK_CTRL |= ((freq_div >> 8) & 0x3) << 6;
    EMMC_CLK_CTRL |= (freq_div & 0xFF) << 8;

    if (EMMC_CLK_CTRL & SDCLK_EN) {
        delay_ms(1);
    }
}

int emmc_init() {
    int ret;

    // SDHOST_Register start
    REG_AP_APB_APB_EB |= 1 << 25; // BIT_AP_APB_EMMC_EB
    emmc_sdclk_enable(0);
    REG_AP_APB_APB_RST |= 1 << 19; // BIT_AP_APB_EMMC_SOFT_RST
    delay_ms(1);
    REG_AP_APB_APB_RST &= ~(1 << 19); // BIT_AP_APB_EMMC_SOFT_RST
    // SDHOST_Register end

    // SDIO_PowerCtl start
    // SDHOST_RST start
    EMMC_CLK_CTRL |= SW_RST_ALL;
    while (EMMC_CLK_CTRL & SW_RST_ALL);
    // SDHOST_RST end

    // SDHOST_ClkFreq_Set start
    REG_AP_CLK_CORE_CGM_EMMC_2X_CFG = 0x03; // AP_CLK_FREQ_384M
    emmc_set_frequency(400000); // 400Khz
    // SDHOST_InternalClk_Enable start
    EMMC_CLK_CTRL |= INT_CLK_EN;
    while (!(EMMC_CLK_CTRL & INT_CLK_STABLE));
    // SDHOST_InternalClk_Enable end
    emmc_sdclk_enable(1);
    // SDIO_Enable64Bit start
    EMMC_HOST_CTRL2 |= _64_BIT_ADDR_EN;
    // SDIO_Enable64Bit end
    // SDIO_SetDmaMode start
    EMMC_HOST_CTRL1 &= ~(0x18); // DMA_SEL(SDMA)
    // SDIO_SetDmaMode end

    // CARD_SDIO_InitCard start
    ret = emmc_send_cmd(EMMC_CMD_GO_IDLE_STATE, 0, NULL);
    if (ret) {
        print("emmc_init: GO_IDLE_STATE failed\r\n");
        return ret;
    }

    uint32_t op_cond_response;
    do {
        ret = emmc_send_cmd(EMMC_CMD_SEND_OP_COND, (1 << 30) | 0x00FF8000, &op_cond_response); // SECTOR_MODE = (1 << 30)
        if (ret) {
            print("emmc_init: SEND_OP_COND failed\r\n");
            return ret;
        }
    } while (!(op_cond_response & (1 << 31)));

    ret = emmc_send_cmd(EMMC_CMD_ALL_SEND_CID, 0, NULL);
    if (ret) {
        print("emmc_init: ALL_SEND_CID failed\r\n");
        return ret;
    }

    emmc_send_cmd(EMMC_CMD_SET_RELATIVE_ADDR, 1 << 16, NULL);
    if (ret) {
        print("emmc_init: SET_RELATIVE_ADDR failed\r\n");
        return ret;
    }

    // TODO: Spec says to check here for the SPEC_VERS of the card with CMD9 before sending SEND_EXT_CSD

    ret = emmc_send_cmd(EMMC_CMD_SELECT_DESELECT_CARD, 1 << 16, NULL);
    if (ret) {
        print("emmc_init: SELECT_DESELECT_CARD failed\r\n");
        return ret;
    }

    uint8_t ext_csd[512];
    ret = emmc_send_cmd_data(EMMC_CMD_SEND_EXT_CSD, 0, ext_csd, 512, 1, NULL);
    if (ret) {
        print("emmc_init: SEND_EXT_CSD failed\r\n");
        return ret;
    }

    sec_count = ext_csd[212] | (ext_csd[213] << 8) | (ext_csd[214] << 16) | (ext_csd[215] << 24);
    print("emmc_init: sec_count = ");
    print_dec(sec_count);
    print("\r\n");

    // TODO: Replace with emmc_switch
    ret = emmc_send_cmd(EMMC_CMD_SWITCH, 0x03B90100, NULL);
    if (ret) {
        // TODO: Specify what was being set
        print("emmc_init: SWITCH failed\r\n");
        return ret;
    }

    ret = emmc_switch(185, 1, NULL); // HS_TIMING, 52Mhz
    if (ret) {
        // TODO: Specify what was being set
        print("emmc_init: SWITCH failed\r\n");
        return ret;
    }

    emmc_set_frequency(48000000); // 48Mhz

    ret = emmc_switch(183, 2, NULL); // BUS_WIDTH, 8 bit
    if (ret) {
        // TODO: Specify what was being set
        print("emmc_init: SWITCH failed\r\n");
        return ret;
    }

    EMMC_HOST_CTRL1 |= 1 << 5; // SD8_MODE

    ret = emmc_send_cmd(EMMC_CMD_SET_BLOCKLEN, 512, NULL);
    if (ret) {
        print("emmc_init: SET_BLOCKLEN failed\r\n");
        return ret;
    }

    return ret;
}