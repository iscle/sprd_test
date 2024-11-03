#pragma once

#define readb(addr) (*(volatile uint8_t *)(addr))
#define writeb(addr, val) (*(volatile uint8_t *)(addr) = (val))
#define readw(addr) (*(volatile uint16_t *)(addr))
#define writew(addr, val) (*(volatile uint16_t *)(addr) = (val))
#define readl(addr) (*(volatile uint32_t *)(addr))
#define writel(addr, val) (*(volatile uint32_t *)(addr) = (val))
#define readq(addr) (*(volatile uint64_t *)(addr))
#define writeq(addr, val) (*(volatile uint64_t *)(addr) = (val))

#define WRITE_BE8(p, val) do { \
    ((uint8_t *)(p))[0] = (uint8_t)(val); \
} while (0)

#define WRITE_BE16(p, val) do { \
    ((uint8_t *)(p))[0] = (uint8_t)((val) >> 8); \
    ((uint8_t *)(p))[1] = (uint8_t)(val); \
} while (0)

#define WRITE_BE32(p, val) do { \
    ((uint8_t *)(p))[0] = (uint8_t)((val) >> 24); \
    ((uint8_t *)(p))[1] = (uint8_t)((val) >> 16); \
    ((uint8_t *)(p))[2] = (uint8_t)((val) >> 8); \
    ((uint8_t *)(p))[3] = (uint8_t)(val); \
} while (0)

#define WRITE_BE64(p, val) do { \
    ((uint8_t *)(p))[0] = (uint8_t)((val) >> 56); \
    ((uint8_t *)(p))[1] = (uint8_t)((val) >> 48); \
    ((uint8_t *)(p))[2] = (uint8_t)((val) >> 40); \
    ((uint8_t *)(p))[3] = (uint8_t)((val) >> 32); \
    ((uint8_t *)(p))[4] = (uint8_t)((val) >> 24); \
    ((uint8_t *)(p))[5] = (uint8_t)((val) >> 16); \
    ((uint8_t *)(p))[6] = (uint8_t)((val) >> 8); \
    ((uint8_t *)(p))[7] = (uint8_t)(val); \
} while (0)

#define READ_BE8(p) \
    ((uint8_t)((const uint8_t *)(p))[0])

#define READ_BE16(p) \
    (((uint16_t)((const uint8_t *)(p))[0] << 8) | \
     ((uint16_t)((const uint8_t *)(p))[1]))

#define READ_BE32(p) \
    (((uint32_t)((const uint8_t *)(p))[0] << 24) | \
     ((uint32_t)((const uint8_t *)(p))[1] << 16) | \
     ((uint32_t)((const uint8_t *)(p))[2] << 8) | \
     ((uint32_t)((const uint8_t *)(p))[3]))

#define READ_BE64(p) \
    (((uint64_t)((const uint8_t *)(p))[0] << 56) | \
     ((uint64_t)((const uint8_t *)(p))[1] << 48) | \
     ((uint64_t)((const uint8_t *)(p))[2] << 40) | \
     ((uint64_t)((const uint8_t *)(p))[3] << 32) | \
     ((uint64_t)((const uint8_t *)(p))[4] << 24) | \
     ((uint64_t)((const uint8_t *)(p))[5] << 16) | \
     ((uint64_t)((const uint8_t *)(p))[6] << 8) | \
     ((uint64_t)((const uint8_t *)(p))[7]))
