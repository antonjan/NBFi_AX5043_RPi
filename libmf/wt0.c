#include "wtimer.h"

uint8_t wtimer0_removecb_core(struct wtimer_desc *desc)
{
	struct wtimer_desc *d;
	uint8_t ret = 0;
	d = WTIMER_PTR(wtimer_state[0].queue);
	for (;;) {
		struct wtimer_desc *dn = d->next;
		if (dn == WTIMER_NULLDESC)
			break;
		if (dn == desc) {
			dn = dn->next;
			d->next = dn;
			++ret;
			if (dn == WTIMER_NULLDESC)
				break;
		}
		d = dn;
	}
	return ret;
}

void wtimer0_addabsolute(struct wtimer_desc *desc)
{
	wtimer0_update();
	wtimer0_addcore(desc);
	wtimer0_schedq();
}

void wtimer0_addrelative(struct wtimer_desc *desc)
{
	wtimer0_update();
	desc->time += wtimer_state[0].time;
	wtimer0_addcore(desc);
	wtimer0_schedq();
}

uint64_t wtimer0_curtime(void)
{
	wtimer0_update();
	return wtimer_state[0].time;
}

uint8_t wtimer0_remove(struct wtimer_desc *desc)
{
	uint8_t ret;

	ret = wtimer_removecb_core((struct wtimer_callback *)desc);
	ret += wtimer0_removecb_core(desc);
	return ret;
}
