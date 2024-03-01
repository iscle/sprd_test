#ifndef EMMC_H
#define EMMC_H

#include <stdint.h>

typedef enum {
    EMMC_CMD_GO_IDLE_STATE,
    EMMC_CMD_SEND_OP_COND,
    EMMC_CMD_ALL_SEND_CID,
    EMMC_CMD_SET_RELATIVE_ADDR,
    EMMC_CMD_SET_DSR,
    EMMC_CMD_SLEEP_AWAKE,
    EMMC_CMD_SWITCH,
    EMMC_CMD_SELECT_DESELECT_CARD,
    EMMC_CMD_SEND_EXT_CSD,
    EMMC_CMD_SEND_CSD,
    EMMC_CMD_SEND_CID,
    EMMC_CMD_READ_DAT_UNTIL_STOP,
    EMMC_CMD_STOP_TRANSMISSION,
    EMMC_CMD_SEND_STATUS,
    EMMC_CMD_BUSTEST_R,
    EMMC_CMD_GO_INACTIVE_STATE,
    EMMC_CMD_SET_BLOCKLEN,
    EMMC_CMD_READ_SINGLE_BLOCK,
    EMMC_CMD_READ_MULTIPLE_BLOCK,
    EMMC_CMD_BUSTEST_W,
    EMMC_CMD_WRITE_DAT_UNTIL_STOP,
    EMMC_CMD_SET_BLOCK_COUNT,
    EMMC_CMD_WRITE_BLOCK,
    EMMC_CMD_WRITE_MULTIPLE_BLOCK,
    EMMC_CMD_PROGRAM_CID,
    EMMC_CMD_PROGRAM_CSD,
    EMMC_CMD_SET_WRITE_PROT,
    EMMC_CMD_CLR_WRITE_PROT,
    EMMC_CMD_SEND_WRITE_PROT,
    EMMC_CMD_ERASE_GROUP_START,
    EMMC_CMD_ERASE_GROUP_END,
    EMMC_CMD_ERASE,
    EMMC_CMD_FAST_IO,
    EMMC_CMD_GO_IRQ_STATE,
    EMMC_CMD_LOCK_UNLOCK,
    EMMC_CMD_APP_CMD,
    EMMC_CMD_GEN_CMD,
} emmc_cmd_t;

int emmc_send_cmd(emmc_cmd_t cmd, uint32_t argument, uint32_t *rsp_buf);
int emmc_send_cmd_data(emmc_cmd_t cmd, uint32_t argument, uint8_t *blocks, uint32_t blk_size, uint32_t blk_count, uint32_t *rsp_buf);
int emmc_init();
int emmc_read_partition(char *partition, void (*start_cb)(uint64_t len), void (*data_cb)(const void *buf, uint16_t len), void (*end_cb)());

#endif //EMMC_H
