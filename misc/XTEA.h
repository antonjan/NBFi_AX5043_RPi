#ifndef XTEA_H
#define XTEA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libmf/libmftypes.h>
#include <stdbool.h>
#include <stdint.h>

void XTEA_Encode(uint8_t* buf);

void XTEA_Decode(uint8_t* buf);

_Bool XTEA_Available();

_Bool XTEA_Enabled();

void XTEA_Enable(_Bool enable);

void XTEA_Set_KEY(uint32_t* key);

void XTEA_Set_KEY_Ptr(uint32_t* ptr);

#ifdef __cplusplus
}
#endif

#endif
