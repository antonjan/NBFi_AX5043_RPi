#ifndef WTIMER_H
#define WTIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "libmfwtimer.h"

#define WTIMER_NULL		(0x0)
#define WTIMER_NULLDESC	((struct wtimer_desc *)WTIMER_NULL)
#define WTIMER_NULLCB	((struct wtimer_callback *)WTIMER_NULL)
#define WTIMER_PTR(x)	((struct wtimer_desc *)((char *)&(x) - (char *)&((struct wtimer_desc *)0)->next))
#define WTIMER_CBPTR(x)	((struct wtimer_callback *)((char *)&(x) - (char *)&((struct wtimer_callback *)0)->next))

struct wtimer_state {
	uint64_t time;
	struct wtimer_desc *queue;
};

extern struct wtimer_state wtimer_state[2];
extern struct wtimer_callback *wtimer_pending;

// the following routines must be called with (wtimer) interrupts disabled
extern void wtimer_addcb_core(struct wtimer_callback *desc);
extern uint8_t wtimer_removecb_core(struct wtimer_callback *desc);
extern void wtimer0_schedq(void);
extern void wtimer0_update(void);
extern void wtimer0_addcore(struct wtimer_desc *desc);
extern uint8_t wtimer0_removecb_core(struct wtimer_desc *desc);


#ifdef __cplusplus
}
#endif

#endif /* WTIMER_H */
