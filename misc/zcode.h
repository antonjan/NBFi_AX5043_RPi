#ifndef ZCODE_H
#define ZCODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libmf/libmftypes.h>
#include <stdbool.h>
#include <stdint.h>

#define ZCODE_LEN 16

void ZCODE_Append(uint8_t * src_buf, uint8_t * dst_buf, _Bool parity);

#ifdef __cplusplus
}
#endif

#endif // ZCODE_H
