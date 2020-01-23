#include "wtimer.h"

uint8_t wtimer_remove(struct wtimer_desc *desc)
{
	uint8_t ret;
	
	ret = wtimer_removecb_core((struct wtimer_callback *)desc);
	ret += wtimer0_removecb_core(desc);

	return ret;
}

void wtimer_add_callback(struct wtimer_callback *desc)
{
	wtimer_addcb_core(desc);
}

uint8_t wtimer_remove_callback(struct wtimer_callback *desc)
{
	return wtimer_removecb_core(desc);
}

uint8_t wtimer_removecb_core(struct wtimer_callback *desc)
{
	struct wtimer_callback *d;
	uint8_t ret = 0;
	d = WTIMER_CBPTR(wtimer_pending);
	for (;;) {
		struct wtimer_callback *dn = d->next;
		if (dn == WTIMER_NULLCB)
			break;
		if (dn == desc) {
			dn = dn->next;
			d->next = dn;
			++ret;
			if (dn == WTIMER_NULLCB)
				break;
		}
		d = dn;
	}
	return ret;
}
