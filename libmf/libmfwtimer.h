#ifndef LIBMFWTIMER_H
#define LIBMFWTIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifdef WTIMER_TIM_CLK
#define SECONDS(x) 		((uint32_t)(x) * WTIMER_TIM_CLK)
#else
#define SECONDS(x) 		((uint32_t)(x) * 1000)
#endif
#define MILLISECONDS(x) (SECONDS(x) / 1000)

#define ABSOLUTE	0
#define RELATIVE	1

struct wtimer_callback;
struct wtimer_desc;

typedef void (*wtimer_callback_handler_t)(struct wtimer_callback *desc);
typedef void (*wtimer_desc_handler_t)(struct wtimer_desc *desc);

struct wtimer_callback {
	// do not change the order
	// must be compatible with wtimer_desc
	struct wtimer_callback *next;
	wtimer_callback_handler_t handler;
};

struct wtimer_desc {
	// do not change the order
	struct wtimer_desc *next;
	wtimer_desc_handler_t handler;
	uint64_t time;
};

struct wtimer_fn_t
{
	uint64_t (*get_time)(void);
};

uint8_t CheckTask(struct wtimer_desc *desc);
void ScheduleTask(struct wtimer_desc *desc, wtimer_desc_handler_t handler, uint8_t relative, uint32_t time);

extern void wtimer_init(void);
extern void wtimer_reg_func(struct wtimer_fn_t *fn);
extern uint8_t wtimer_runcallbacks(void);

extern uint64_t wtimer0_curtime(void);
extern void wtimer0_addabsolute(struct wtimer_desc *desc);
extern void wtimer0_addrelative(struct wtimer_desc *desc);
extern uint8_t wtimer_remove(struct wtimer_desc *desc);
extern uint8_t wtimer0_remove(struct wtimer_desc *desc);

extern void wtimer_add_callback(struct wtimer_callback *desc);
extern uint8_t wtimer_remove_callback(struct wtimer_callback *desc);

#ifdef __cplusplus
}
#endif

#endif /* LIBMFWTIMER_H */
