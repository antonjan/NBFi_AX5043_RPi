#ifndef LIBMFTYPES_H
#define LIBMFTYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern int32_t signextend12(int16_t x);
extern int32_t signextend16(int16_t x);
extern int32_t signextend20(int32_t x);
extern int32_t signextend24(int32_t x);
extern uint8_t hweight8(uint8_t x);
extern uint8_t hweight16(uint16_t x);
extern uint8_t hweight32(uint32_t x);
#define parity8(x) (hweight8(x) & 1)
#define parity16(x) (hweight16(x) & 1)
#define parity32(x) (hweight32(x) & 1)

extern int16_t signedlimit16(int16_t x, int16_t lim);
extern uint8_t checksignedlimit16(int16_t x, int16_t lim);
extern int32_t signedlimit32(int32_t x, int32_t lim);
extern uint8_t checksignedlimit32(int32_t x, int32_t lim);
extern uint8_t gray_encode8(uint8_t x);
extern uint8_t gray_decode8(uint8_t x);

/*
 * Reverse Bits
 */
extern uint8_t rev8(uint8_t x);

/*
 * fast memset and memcpy
 */
void fmemset(void *p, char c, uint16_t n);
void fmemcpy(void *d, const void *s, uint16_t n);

#ifdef __cplusplus
}
#endif

#endif /* LIBMFTYPES_H */
