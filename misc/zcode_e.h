#ifndef ZCODE_E_H
#define ZCODE_E_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libmf/libmftypes.h>
#include <stdbool.h>
#include <stdint.h>

#define ZCODE_E_LEN 32

void ZCODE_E_Append(uint8_t * src_buf, uint8_t * dst_buf, _Bool parity);

#ifdef __cplusplus
}
#endif

#endif // ZCODE_E_H
